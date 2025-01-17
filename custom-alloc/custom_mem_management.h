#ifndef CUSTOM_MEM_MANAGMENT_H
#define CUSTOM_MEM_MANAGMENT_H

#include "custom_alloc.h"

void free(void* ptr);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nelem, size_t elsize);

#endif // !CUSTOM_MEM_MANAGMENT_H