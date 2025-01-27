#include "custom_alloc.h"

meta_data first_fit(meta_data *prev, size_t size)
{
    // implement first fit algorithm

    meta_data current = heap_head;
    while (current && !(current->free && current->size >= size))
    {
        *prev = current;
        current = current->next;
    }

    return current;

}