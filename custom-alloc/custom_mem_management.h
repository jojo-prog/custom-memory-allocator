#ifndef CUSTOM_MEM_MANAGMENT_H
#define CUSTOM_MEM_MANAGMENT_H

#include "custom_alloc.h"

void custom_free(void* ptr);
void *custom_realloc(void *ptr, size_t size);
void *custom_calloc(size_t nelem, size_t elsize);

#endif // !CUSTOM_MEM_MANAGMENT_H