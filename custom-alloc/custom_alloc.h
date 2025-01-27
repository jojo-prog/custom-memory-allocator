#ifndef CUSTOM_ALLOC_H
#define CUSTOM_ALLOC_H
#define _BSD_SOURCE
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
 /**
     * Aligns the given size to the nearest multiple of 4.
     *
     * @param size The size to be aligned.
     * @return The size aligned to the nearest multiple of 4.
 */

#define PAGE_SIZE sysconf(_SC_PAGESIZE)
#define MEM_ALLOC_LOT_SIZE (1 * PAGE_SIZE)
#define MEM_DEALLOC_LOT_SIZE (2 * PAGE_SIZE)
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
    size_t size;               // Block size
    meta_data next;    // Next free block
    meta_data prev;    // Previous free block (optional)
    void* ptr;          // Pointer to the memory block
    unsigned int free;     // 1-bit for free status
    
};


#define META_DATA_SIZE sizeof(struct meta_data)
#define WRITABLE_AREA(p) (((meta_data )p) + 1)
#define HEADER_AREA(p) (((meta_data )p) - 1)
#define MAX(X, Y) (((size_t)(X) > (size_t)(Y)) ? (size_t)(X) : (size_t)(Y))

meta_data mem_pool = NULL;

int brk(void *addr);
void *sbrk(intptr_t increment);

void *custom_malloc(size_t size, meta_data find_free_block(meta_data *prev, size_t size));
void custom_free(void *ptr);
void *custom_realloc(void *ptr, size_t size, meta_data find_free_block(meta_data *prev, size_t size));
void *custom_calloc(size_t nelem, size_t elsize, meta_data find_free_block(meta_data *prev, size_t size));

int is_valid_addr(void *p);


#endif // !CUSTOM_ALLOC_H