#include "custom_alloc.h"

// Aligns the given size to the nearest multiple of alignment

void add_mem_to_pool(meta_data mem)
{
  if (mem_pool == NULL)
  {
    mem_pool = mem;
    mem_pool->next = NULL;
    mem_pool->prev = NULL;
    return;
  }

  meta_data parent = mem_pool;
  while (is_valid_addr(parent)&&parent->next != NULL)
    parent = parent->next;

  parent->next = mem;
  mem->prev = parent;
  mem->next = NULL;
}


/**
 * @brief Splits a larger memory block into a smaller block of the requested size.
 *
 * This function takes a larger memory block and splits it into a smaller block
 * of the specified size. The remaining part of the larger block is kept as a
 * free block.
 *
 * @param block The larger memory block to be split.
 * @param size The size of the smaller block to be created.
 */
void split_block(meta_data block, size_t size)
{
  // Calculate the address of the new block
  void *new_block_address = (void *)((char *)block + size + META_DATA_SIZE);

  // Initialize the new block
  meta_data new_block = (meta_data)new_block_address;
  new_block->size = block->size - size - META_DATA_SIZE;
  new_block->free = 1; // Mark as free
  new_block->next = block->next;
  new_block->prev = block;
  new_block->ptr = (void *)(new_block + 1);

  // Update the original block
  block->size = size;
  block->next = new_block;

  // Update the next block's prev pointer if it exists
  if (new_block->next)
  {
    new_block->next->prev = new_block;
  }
}


/**
 * @brief Merges consecutive free memory blocks into a single block.
 *
 * This function iterates through the memory pool and merges adjacent free
 * memory blocks into a single larger block to reduce fragmentation.
 */
void merge_free_blocks()
{
  meta_data ptr = mem_pool;

  if (ptr == NULL)
  {
    return;
  }

  while (ptr && ptr->next)
  {

    if (ptr->free == 1 && ptr->next->free == 1)
    {
      ptr->size += ptr->next->size + META_DATA_SIZE;
      ptr->next = ptr->next->next;
      if (ptr->next != NULL)
        ptr->next->prev = ptr;
    }
    else
    {
      ptr = ptr->next;
    }
  }
}

void release_memory_if_required()
{

  meta_data ptr = mem_pool;
  meta_data free_area_start = NULL;
  size_t totalFreeSpace = 0;

  while (ptr)
  {
    if (ptr->free)
    {
      free_area_start = ptr;
      totalFreeSpace += ptr->size;
    }
    else
    {
      free_area_start = NULL;
      //FIXME: totalFreeSpace = 0; // this line is not required
    }
    ptr = ptr->next;
  }

  if (free_area_start == NULL || totalFreeSpace < (size_t)MEM_DEALLOC_LOT_SIZE)
  {
    return;
  }

  void *reset = NULL;

  if (free_area_start == mem_pool)
  {
    reset = mem_pool;
    mem_pool = NULL;
  }
  else
  {
    reset = free_area_start;
    free_area_start->prev->next = NULL;
  }
  brk(reset);
}

/**
 * @brief Inserts a new memory block into the linked list of memory blocks.
 *
 * This function inserts a new memory block of the specified size into the
 * linked list of memory blocks, immediately following the given previous
 * block. The new block's metadata is returned.
 *
 * @param prev The metadata of the previous memory block in the linked list.
 * @param size The size of the new memory block to be inserted.
 * @return The metadata of the newly inserted memory block.
 */
meta_data allocate_mem(meta_data prev, size_t size)
{
  void *start_meta_data = sbrk(0); // get current break address
  if (start_meta_data == (void *)-1)
  {
    return NULL;
  }

  void *end_address = sbrk(size + META_DATA_SIZE); // increment break address by size
  if (end_address == (void *)-1)
  {
    return NULL;
  }

  meta_data block = start_meta_data;
  block->size = size;
  block->next = NULL;
  block->prev = prev;
  if (block)
  {
    block->free = 0;
  }
  block->ptr = block + 1;
  if (prev)
  {
    prev->next = block;
  }
  return block;
}

/**
 * @brief Allocates a block of memory of the specified size.
 *
 * This function attempts to allocate a block of memory of the given size
 * using a custom memory allocation strategy. If the allocation is successful,
 * a pointer to the beginning of the block is returned. If the allocation fails,
 * a NULL pointer is returned.
 *
 * @param size The size of the memory block to allocate, in bytes.
 * @param find_free_block A function pointer to find a free memory block.
 * @return void* A pointer to the allocated memory block, or NULL if the allocation fails.
 */
void *custom_malloc(size_t size, meta_data (*find_free_block)(meta_data *prev, size_t size))
{
  meta_data mem = NULL;
  meta_data prev = NULL;
  size_t msize = size + META_DATA_SIZE;

  // check memory-pool if any free memory available
  mem = find_free_block(&prev, msize);

  if (size == 0) // special case, check man page.
    return mem;

  if (mem == NULL) // if no free memory available in memory-pool
  {
    // allocate big chunk memory at once. Max of (Multiple of PAGE_SIZE,  MEM_ALLOC_LOT_SIZE)
    size_t allocate_size = MAX(((msize / PAGE_SIZE) + 1) * PAGE_SIZE, MEM_ALLOC_LOT_SIZE);

    if ((mem = allocate_mem(prev, allocate_size)) == NULL)
    {
      return NULL;
    }

    add_mem_to_pool(mem); // add the memory to memory-pool
  } else {
    if (mem->size > msize)
    {
      split_block(mem, msize);
    }
  }

  // maybe user wants 20 bytes data but
  // we may created/have big chunk of memory
  // So, we'll resize the memory.


  mem->free = 0;
  return WRITABLE_AREA(mem);
}

int is_valid_addr(void *p)
{
  if (mem_pool)
  {
    if (p > (void*) mem_pool && p < sbrk(0))
    {
      void *ptr = HEADER_AREA(p)->ptr;
      int res = ptr == p;
      return res;
    }
  }
  return 0;
}

/**
 * @brief Frees the memory space pointed to by ptr, which must have been returned by a previous call to custom_malloc, custom_calloc, or custom_realloc.
 *
 * This function does not return a value. The memory space is made available for future allocations.
 *
 * @param ptr Pointer to the memory block to be freed. If ptr is NULL, no operation is performed.
 */

void custom_free(void *ptr)
{
  if (ptr == NULL)
    return;
  if (!is_valid_addr(ptr))
    return;

  meta_data block = HEADER_AREA(ptr);
  block->free = 1;

  // after free, merge the small free chunks, so that
  merge_free_blocks();

  // if a big chunk of data is unused,
  // then back the memory to system.
  release_memory_if_required();
}

void *custom_realloc(void *ptr, size_t size, meta_data find_free_block(meta_data *prev, size_t size))
{
  if (!ptr)
  {
    return custom_malloc(size, find_free_block);
  }
  if (!is_valid_addr(ptr))
  {
    return NULL;
  }
  meta_data block = HEADER_AREA(ptr);
  if (block->size >= size)
  {
    return ptr;
  }
  void *new_ptr;
  new_ptr = custom_malloc(size, find_free_block);
  if (!new_ptr)
  {
    return NULL;
  }
  memcpy(new_ptr, ptr, block->size);
  custom_free(ptr);
  return new_ptr;
}

void *custom_calloc(size_t nelem, size_t elsize, meta_data find_free_block(meta_data *prev, size_t size))
{
  size_t size = nelem * elsize;
  void *ptr = custom_malloc(size, find_free_block);
  memset(ptr, 0, size);
  return ptr;
}
