#include "custom_alloc.h"


/**
 * @brief Finds a free memory block of at least the specified size.
 *
 * This function traverses the linked list of memory blocks starting from the 
 * head of the heap to find a free block that is large enough to accommodate 
 * the requested size. If such a block is found, it is returned; otherwise, 
 * the function returns NULL.
 *
 * @param prev A pointer to a meta_data pointer that will be updated to point 
 *             to the previous block in the list before the found free block.
 * @param size The size of the memory block required.
 * @return A meta_data pointer to the found free block, or NULL if no suitable 
 *         block is found.
 */
// fitst fit
meta_data find_free_block(meta_data *prev, size_t size)
{
    meta_data current = heap_head;
    while (current && !(current->free && current->size >= size))
    {  
        *prev = current;
        current = current->next;
    }
    return current;
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
    void* new_block_address = (void*)((char*)block + size + META_DATA_SIZE);

    // Initialize the new block
    meta_data new_block = (meta_data)new_block_address;
    new_block->size = block->size - size - META_DATA_SIZE;
    new_block->free = 1; // Mark as free
    new_block->next = block->next;
    new_block->prev = block;
    new_block->ptr = (void*)(new_block + 1);

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
meta_data insert_block(meta_data prev, size_t size)
{
    void* start_meta_data = sbrk(0); // get current break address
    if (start_meta_data == (void*)-1)
    {
        return NULL;
    }

    void* end_address = sbrk(size + META_DATA_SIZE); // increment break address by size
    if (end_address == (void*)-1)
    {
        return NULL;
    }

    meta_data block = start_meta_data;
    block->size = size;
    block->next = NULL;
    block->prev = prev;
    if (block) {
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
 * @return void* A pointer to the allocated memory block, or NULL if the allocation fails.
 */
void *custom_malloc(size_t size, meta_data find_free_block(meta_data *prev, size_t size))
{
    if (size <= 0)
    {
        return NULL;
    }
    size_t s;
    s = align4(size);

    meta_data block = NULL;

    if (!heap_head)
    {
        // create the first block
        block = insert_block(NULL, s);
        if (!block)
        {
            return NULL;
        }
        heap_head = block;
    }
    else
    {
       // find a free block
        meta_data prev = NULL;
        block = find_free_block(&prev, s);
        if (!block)
        {
            // no free block found, create a new block
            block = insert_block(prev, s);
            if (!block)
            {
                return NULL;
            }
        }
        else
        {
           block->free = 0;
           // implement splitting of the block
           if (block->size > s + META_DATA_SIZE)
            {
                split_block(block, s);
            }
        }
        
    }
    return block + 1;
}

/**
 * @brief Merges adjacent free memory blocks into a single block.
 *
 * This function takes a memory block's metadata and merges it with any 
 * adjacent free blocks to create a larger contiguous block of free memory.
 *
 * @param block The metadata of the memory block to be merged.
 * @return The metadata of the merged memory block.
 */
meta_data merge_blocks(meta_data block)
{
  if (block && block->next && block->next->free)
  {
    block->size += META_DATA_SIZE + block->next->size;
    block->next = block->next->next;
  }
  if (block->next)
  {
    block->next->prev = block;
  }
  return block;
}

meta_data get_block_addr(void *p)
{
  return (meta_data)p - 1;
}

int is_valid_addr(void *p)
{
  if (heap_head)
  {
    if (p > heap_head && p < sbrk(0))
    {
      void *ptr = (get_block_addr(p))->ptr;
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
  if (!ptr)
  {
    return;
  }
  if (!is_valid_addr(ptr))
  {
    return;
  }
  meta_data block = get_block_addr(ptr);
  if (block->free)
  {
    return;
  }
  block->free = 1;
  if (block->prev && block->prev->free)
  {
    block = merge_blocks(block->prev);
  }

  if (block->next)
  {
    block = merge_blocks(block);
  }
  else
  {
    if (block->prev)
    {
      block->prev->next = NULL;
    }
    else
    {
      heap_head = NULL;
    }
    if ((char *)block + block->size + META_DATA_SIZE == sbrk(0))
    {
        sbrk(0 - (block->size + META_DATA_SIZE));
    }
  }
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
  meta_data block = get_block_addr(ptr);
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










