#include "custom_alloc.h"

meta_data get_block_ptr(void *ptr) {
    return (meta_data)ptr - 1;
}

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

void split_block(meta_data big_block, size_t size)
{
    void* start_meta_data1 = big_block;
    void* start_meta_data2 = big_block + META_DATA_SIZE + size;
    int new_size = big_block->size - size - META_DATA_SIZE;

    meta_data block1 = start_meta_data1;
    block1->size = size;
    block1->next = start_meta_data2;
    block1->prev = big_block->prev;
    block1->free = 0;

    meta_data block2 = start_meta_data2;
    block2->size = new_size;
    block2->next = big_block->next;
    block2->prev = block1;
    block2->free = 1;

    if (block2->next)
    {
        block2->next->prev = block2;
    }



    
}

meta_data insert_block(meta_data prev, size_t size)
{
    void* start_meta_data = sbrk(0); // get current break address
    void* end_address = sbrk(size + META_DATA_SIZE); // increment break address by size
    void* start_address = start_meta_data + META_DATA_SIZE; // start address of the block
    if (end_address == (void*)-1)
    {
        return NULL;
    }

    meta_data block = start_meta_data;
    block->size = size;
    block->next = NULL;
    block->prev = prev;
    block->ptr = start_address;
    block->free = 0;
    if (prev)
    {
        prev->next = block;
    }
    return block;
    
}

void *custom_malloc(size_t size)
{
    size_t s;
    s = align4(size);
    if (size <= 0)
    {
        return NULL;
    }

    meta_data block;

    if (!heap_head)
    {
        // create the first block
        block = insert_block(NULL, size);
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
        block = find_free_block(&prev, size);
        if (!block)
        {
            // no free block found, create a new block
            block = insert_block(prev, size);
            if (!block)
            {
                return NULL;
            }
        }
        else
        {
            if (block->size - s >= (META_DATA_SIZE + 4))
            {
                // if the requested block and another block with size 1 fits in the free block
                split_block(block, size);
            } else {
                block->free = 0;
                // 
            }
        }
    }

    return block +1;

}


