#ifndef CUSTOM_ALLOC_H
#define CUSTOM_ALLOC_H

#include <unistd.h>
#include <stdint.h>

typedef struct block *mem_block;

struct block {
    size_t size;
    mem_block next;
    mem_block prev;
    int free;
    void* ptr;
};

#define BLOCK_SIZE sizeof(struct block)

mem_block head = NULL;
mem_block last_added = NULL;


int brk(void *addr);
void *sbrk(intptr_t increment);

void *custom_malloc(size_t size);

#endif // !CUSTOM_ALLOC_H