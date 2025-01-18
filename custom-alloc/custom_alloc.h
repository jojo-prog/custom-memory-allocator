#ifndef CUSTOM_ALLOC_H
#define CUSTOM_ALLOC_H

#include <unistd.h>
#include <stdint.h>
#include <assert.h>

#define align4(x) (((((x)-1) >> 2) << 2) + 4)

typedef struct meta_data *meta_data;

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