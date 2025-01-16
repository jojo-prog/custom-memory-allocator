#ifndef CUSTOM_MEM_MANAGEMENT_H
#define CUSTOM_MEM_MANAGEMENT_H

#include "custom_alloc.h"


void custom_free(void *ptr);
void *custom_calloc(size_t nmemb, size_t size);
void *custom_realloc(void *ptr, size_t size);

#endif // !CUSTOM_MEM_MANAGEMENT_H