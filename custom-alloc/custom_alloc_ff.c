#include "custom_alloc.h"

meta_data get_block_ptr(void *ptr) {
    return (meta_data)ptr - 1;
}

meta_data find_free_block(meta_data *last, size_t size)
{
    meta_data current = heap_head;
    while (current && !(current->free && current->size >= size))
    {
        *last = current;
        current = current->next;
    }
    return current;
}

void split_block(meta_data block, size_t size)
{
    
}

// allcate size for meta_data of block and request space
meta_data insert_block(meta_data last, size_t size)
{
    
}

void *malloc(size_t size)
{
    return NULL;
}


