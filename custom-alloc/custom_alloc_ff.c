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
 * @param big_block The larger memory block to be split.
 * @param size The size of the smaller block to be created.
 */
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


