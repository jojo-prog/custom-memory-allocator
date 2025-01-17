#include <assert.h>
#include <stdio.h>
#include "custom_alloc.h"
#include "custom_alloc_ff.c"
#include "custom_mem_management.c"

void test_malloc_initial_allocation() {
    void *ptr = malloc(100);
    assert(ptr != NULL);
    free(ptr);
    printf("test_malloc_initial_allocation passed\n");
}

void test_malloc_zero_allocation() {
    void *ptr = malloc(0);
    assert(ptr == NULL);
    free(ptr);
    printf("test_malloc_zero_allocation passed\n");
}

void test_malloc_multiple_allocations() {
    void *ptr1 = malloc(50);
    void *ptr2 = malloc(150);
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);
    assert(ptr1 != ptr2);
    free(ptr1);
    free(ptr2);
    printf("test_malloc_multiple_allocations passed\n");
}

void test_malloc_free_block_reuse() {
    void *ptr1 = malloc(100);
    free(ptr1);
    void *ptr2 = malloc(50);
    assert(ptr1 == ptr2);
    free(ptr2);
    printf("test_malloc_free_block_reuse passed\n");
}

void test_free_already_freed_block() {
    void *ptr = malloc(100);
    free(ptr);
    free(ptr);
    printf("test_free_already_freed_block passed\n");
}

void test_correct_split_block() {
    void *ptr = malloc(100);
    meta_data block = get_block_ptr(ptr);
    split_block(block, 50);
    assert(block->size == 50);
    assert(block->next->size == 50);
    free(ptr);
    printf("test_correct_split_block passed\n");
}

int main() {
    printf("%d\n", META_DATA_SIZE);
    test_malloc_initial_allocation();
    test_malloc_zero_allocation();
    test_malloc_multiple_allocations();
    test_malloc_free_block_reuse();
    test_correct_split_block();
    //test_free_already_freed_block();
    return 0;
}