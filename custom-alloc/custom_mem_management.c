#include "custom_mem_management.h"
#include <string.h>

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
    brk(block);
  }
}

void *custom_realloc(void *ptr, size_t size)
{
  return NULL;
}

void *custom_calloc(size_t nelem, size_t elsize)
{
  return NULL;
}