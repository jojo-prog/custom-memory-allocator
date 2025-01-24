#include "custom_mem_management.h"
#include <string.h>

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

void *custom_realloc(void *ptr, size_t size)
{
  if (!ptr)
  {
    return custom_malloc(size);
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
  new_ptr = custom_malloc(size);
  if (!new_ptr)
  {
    return NULL;
  }
  memcpy(new_ptr, ptr, block->size);
  custom_free(ptr);
  return new_ptr;

}

void *custom_calloc(size_t nelem, size_t elsize)
{
  size_t size = nelem * elsize;
  void *ptr = custom_malloc(size);
  memset(ptr, 0, size);
  return ptr;
}