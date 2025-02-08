#include "custom_alloc.h"
#include <stdio.h>

void check_correct_meta_data(meta_data block)
{
  if (mem_pool == NULL || end_of_pool == NULL)
  {
    return;
  }

  meta_data end = sbrk(0);
  if (block->next == NULL)
  {
    assert(block == end_of_pool);
  }
  else
  {
    assert(block->next >= block);
    assert(block->next < end);
    assert(block->next > mem_pool);
  }
  if (block->prev == NULL)
  {
    assert(block == mem_pool);
  }
  else
  {
    assert(block->prev <= block);
    assert(block->prev < end);
    assert(block->prev >= mem_pool);
  }
  assert(block->size > 0);
  assert(block->size < (size_t)(end - block));
}

// Searches for the smallest free block that is large enough to satisfy the memory request
// the idea is to minimize fragmentation by choosing the smallest possible available block
meta_data best_fit(meta_data *prev, size_t size)
{
  // if no memory has been allocated yet
  if (mem_pool == NULL)
  {
    return NULL;
  }

  meta_data current = mem_pool;
  meta_data best_fit_ptr = NULL;
  // ensures any valid block will be smaller
  size_t min_size = -1;

  while (current)
  {
    // check if the block is free, large enough and smaller than the current
    if (current->free && current->size >= size && current->size < min_size)
    {
      *prev = current->prev;
      best_fit_ptr = current;
      min_size = current->size;
      if (min_size == size)
      {
        break;
      }
    }
    current = current->next;
  }

  return best_fit_ptr;
}

//The Next Fit algorithm works similarly to First Fit, but instead of always searching from the beginning, it resumes searching from the last allocated block.
meta_data next_fit(meta_data *prev, size_t size)
{

  // TODO: implement next fit algorithm

  // if "last_allocated" is NULL, initialize it to the start of the memory pool
  if (last_allocated == NULL)
  {
    last_allocated = mem_pool;
  }

  meta_data current = last_allocated;
  if (mem_pool != last_allocated)
  {
    while (current)
    {
      // if a free block of sufficient size is found
      if (current->free && current->size >= size)
      {
        *prev = current->prev;
        // mark this block as the new starting point for the next allocation
        last_allocated = current;
        return current;
      }
      // if no suitable block is found, the function moves to the next step
      current = current->next;
    }
  }

  // if the first search did not find a suitable block, search from the beginning
  current = last_allocated;
  // stop if "current" reaches "last_allocated"
  while (current)
  {
    if (current->free && current->size >= size)
    {
      *prev = current->prev;
      last_allocated = current;
      return current;
    }
    current = current->prev;
  }

  return NULL;
}

// we traverse the memory pool and allocate the first available block that is large enough to fit the requested size
meta_data first_fit(meta_data *prev, size_t size)
{
  // implement first fit algorithm

  meta_data current = end_of_pool;
  // Loop through the entire linked list until we reach the end (current == NULL)
  while (current)
  {
    if (current->free && current->size >= size)
    {
      *prev = current->prev;
      return current;
    }
    current = current->prev;
  }


  return NULL;

}

/**
 * @brief Adds a memory block to the memory pool.
 *
 * This function takes a memory block described by the meta_data structure
 * and adds it to the memory pool for future allocations.
 *
 * @param mem The memory block to be added to the pool.
 */
void add_mem_to_pool(meta_data mem)
{
  // if "mem_pool" is empty, set it as the First Block
  if (mem_pool == NULL)
  {
    mem_pool = mem;
    mem_pool->next = NULL;
    mem_pool->prev = NULL;
    end_of_pool = mem_pool;
  }
  else
  {
    // add the new block to the end of the memory pool
    end_of_pool->next = mem;
    mem->prev = end_of_pool;
    end_of_pool = mem;
  }
}

meta_data create_new_block(void *memory, meta_data next_block, meta_data prev_block, int status, size_t size)
{
  meta_data block = memory;
  block->size = size;
  block->next = next_block;
  block->prev = prev_block;
  block->free = status;
 // block->ptr = (void *)(block + 1);
  // Update the next block's prev pointer if it exists
  if (block->next)
  {
    block->next->prev = block;
  }
  if (block->prev)
  {
    block->prev->next = block;
  }
  return block;
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

  // compute the address of the new block
  void *new_block_address = (void *)((char *)block + size + META_DATA_SIZE);
  if ((long)(block->size - size - META_DATA_SIZE) < 0)
  {
    return;
  }
  // Initialize the new block
  size_t new_block_size = block->size - size - META_DATA_SIZE;
  meta_data new_block = create_new_block(new_block_address, block->next, block, 1, new_block_size);
  // Update the original block
  block->size = size;

  if (new_block->next == NULL)
  {
    end_of_pool = new_block;
  }

  check_correct_meta_data(block);
  check_correct_meta_data(new_block);

  splits_count++; // useful for performance analysis
}

/**
 * @brief Merges consecutive free memory blocks into a single block
 *
 * This function iterates through the memory pool and merges adjacent free
 * memory blocks into a single larger block to reduce fragmentation.
 */
void merge_blocks(meta_data ptr)
{
  check_correct_meta_data(ptr);

  // If "ptr->next" exists and is free, merge it forward
  if (ptr->next && ptr->next->free)
  {
    ptr->size += ptr->next->size + META_DATA_SIZE;
    ptr->next = ptr->next->next;
    
    if (ptr->next)
    {
      ptr->next->prev = ptr;
    }
    if (ptr->next == NULL)
    {
      end_of_pool = ptr;
    }
  }
  check_correct_meta_data(ptr);
  // If "ptr->prev" exists and is free, merge it backward
  if (ptr->prev && ptr->prev->free)
  {
    ptr->prev->size += ptr->size + META_DATA_SIZE;
    ptr->prev->next = ptr->next;


    if (ptr->next)
    {
      ptr->next->prev = ptr->prev;
    }
    if (ptr == NULL)
    {
      end_of_pool = ptr->prev;
    }
  }
  check_correct_meta_data(ptr);
}

/**
 * @brief Releases memory if certain conditions are met.
 *
 * This function checks specific conditions and releases memory accordingly.
 * The exact conditions and the memory release mechanism should be detailed
 * within the function implementation.
 */
void release_memory_if_required()
{

  meta_data ptr = end_of_pool;
  meta_data free_area_start = NULL;
  size_t totalFreeSpace = 0;
  // computes the total free space available in the memory pool and keeps track of the starting address of the free area
  while (ptr)
  {
    if (ptr->free)
    {
      free_area_start = ptr;
      totalFreeSpace += ptr->size + META_DATA_SIZE;
    }
    else
    {
      free_area_start = NULL;
      totalFreeSpace = 0;
      break;
    }
    ptr = ptr->prev;
  }
  // if the free area is less than the deallocation lot size or no free area exists, return
  if (free_area_start == NULL || totalFreeSpace < (size_t)MEM_DEALLOC_SIZE)
  {
    return;
  }

  void *reset = NULL;

  if (free_area_start == mem_pool)
  {
    reset = mem_pool;
    mem_pool = NULL;
    last_allocated = NULL;
    end_of_pool = NULL;
    end_of_pool = NULL;
  }
  else
  {

    reset = free_area_start;

    if (free_area_start->prev)
    {
      free_area_start->prev->next = NULL;
    }
    end_of_pool = free_area_start->prev;
    end_of_pool = free_area_start->prev;
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

  void *memory = sbrk(size + META_DATA_SIZE); // Expands the heap space by "size + META_DATA_SIZE" bytes
  // if sbrk() fails
  if (memory == (void *)-1)
  {
    return NULL;
  }

  meta_data block = create_new_block(memory, NULL, prev, 0, size);
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
  size_t s = ALING(size, 4); // align the size to 4 bytes for better memory access efficiency

  // check memory-pool if any free memory available
  mem = find_free_block(&prev, s);

  if (size == 0)
    return mem;

  if (mem == NULL) // if no free memory available in memory-pool
  {
    
    // allocate big chunk memory at once. Max of (Multiple of PAGE_SIZE,  MEM_ALLOC_LOT_SIZE)
    /*
    size_t x = (s / PAGE_SIZE) + 1;
    size_t prealloc_size = x * PAGE_SIZE; // makes sure that the size is multiple of PAGE_SIZE and not less than page size
    size_t allocate_size = MAX(prealloc_size, MEM_ALLOC_SIZE);
    */
    
    //"allocate_mem()" to request memory from the OS
    if ((mem = allocate_mem(prev, s)) == NULL)
    {
      return NULL;
    }

    add_mem_to_pool(mem); // add the memory to memory-pool
  }

  // if the block is larger than the requested size, split it
  split_block(mem, s);

  // marks the block as allocated
  mem->free = 0;
  return WRITABLE_AREA(mem);
}

// This function checks if a given pointer p is a valid memory address allocated by our custom memory allocator
int is_valid_addr(void *p)
{
  if (mem_pool)
  {
    // ensures our pointer is within "mem_pool", which is the start of our managed heap and "sbrk(0)", which is the current program break (end of allocated heap)
    if (p > (void *)mem_pool && p < sbrk(0))
    {
      // validate the adress
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
  // check if the pointer is valid
  if (!is_valid_addr(ptr))
    return;

  meta_data block = HEADER_AREA(ptr); // retrieves the metadata block for "ptr"
  block->free = 1;                    // marks the memory block as available

  merge_blocks(block); // merge adjacent free blocks

  release_memory_if_required();
  // memset(ptr, 0, block->size);

  // Free the entire Heap if no memory is allocated
  if (mem_pool != NULL) // if first block is free
  {
    if (mem_pool->free)
    {
      if (mem_pool->next == NULL)
      {
        brk(mem_pool); // release the entire heap back to the OS
        mem_pool = NULL;
        last_allocated = NULL; // no memory is allocated
      }
    }
  }
}

// This function resizes a previously allocated memory block. If necessary, it moves the block to a new location
void *custom_realloc(void *ptr, size_t size, meta_data find_free_block(meta_data *prev, size_t size))
{
  // If "ptr" is NULL, realloc() behaves like malloc(size)
  if (!ptr)
  {
    return custom_malloc(size, find_free_block);
  }
  if (!is_valid_addr(ptr))
  {
    return NULL;
  }
  // If the existing block is large enough, return the same block
  meta_data block = HEADER_AREA(ptr);
  if (block->size >= size)
  {
    return ptr;
  }
  void *new_ptr;
  // If the block is too small, allocate a new one with the requested size
  new_ptr = custom_malloc(size, find_free_block);
  if (!new_ptr)
  {
    return NULL;
  }
  memcpy(new_ptr, ptr, block->size); // Copy the data from the old block to the new block
  custom_free(ptr);                  // Free the old block
  return new_ptr;
}

// This function allocates memory for an array and initializes it to zero
//@param "nelem" number of elements and "elsize" size of each element
void *custom_calloc(size_t nelem, size_t elsize, meta_data find_free_block(meta_data *prev, size_t size))
{

  size_t size = nelem * elsize;                     // calculate the total size of the array
  void *ptr = custom_malloc(size, find_free_block); // allocate the required memory
  memset(ptr, 0, size);                             // initialize the memory to zero

  return ptr;
}
