#ifndef CUSTOM_ALLOC_H
#define CUSTOM_ALLOC_H

#include <unistd.h>
#include <stdint.h>
#include <assert.h>
 /**
     * Aligns the given size to the nearest multiple of 4.
     *
     * @param size The size to be aligned.
     * @return The size aligned to the nearest multiple of 4.
 */
#define align4(x) (((((x)-1) >> 2) << 2) + 4)

typedef struct meta_data *meta_data;

/**
 * struct meta_data - Structure to hold metadata for custom memory allocator.
 * @free: Indicates if the block is free (1) or allocated (0).
 * @size: Size of the memory block.
 * @next: Pointer to the next metadata block in the linked list.
 * @prev: Pointer to the previous metadata block in the linked list.
 * @ptr: Pointer to the actual memory block.
 */
struct meta_data
{
    int free;
    size_t size;
    meta_data next;
    meta_data prev;
    void *ptr;

};

#define META_DATA_SIZE sizeof(struct meta_data)

void *heap_head = NULL;

int brk(void *addr);
void *sbrk(intptr_t increment);

void *custom_malloc(size_t size);

#endif // !CUSTOM_ALLOC_H