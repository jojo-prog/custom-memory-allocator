#include <assert.h>
#include <stdio.h>
#include "custom_alloc.h"
#include "custom_alloc.c"
#include "custom_mem_management.c"

void test_malloc_initial_allocation() {
    void *ptr = custom_malloc(100);
    assert(ptr != NULL);
    custom_free(ptr);
    printf("test_malloc_initial_allocation passed\n");
}

void test_malloc_zero_allocation() {
    void *ptr = custom_malloc(0);
    assert(ptr == NULL);
    custom_free(ptr);
    printf("test_malloc_zero_allocation passed\n");
}

void test_malloc_multiple_allocations() {
    void *ptr1 = custom_malloc(50);
    void *ptr2 = custom_malloc(150);
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);
    assert(ptr1 != ptr2);
    custom_free(ptr1);
    custom_free(ptr2);
    printf("test_malloc_multiple_allocations passed\n");
}

void test_malloc_free_block_reuse() {
    void *ptr1 = custom_malloc(100);
    custom_free(ptr1);
    void *ptr2 = custom_malloc(50);
    assert(ptr1 == ptr2);
    custom_free(ptr2);
    printf("test_malloc_free_block_reuse passed\n");
}

void test_free_already_freed_block() {
    void *ptr = custom_malloc(100);
    custom_free(ptr);
    custom_free(ptr);
    printf("test_free_already_freed_block passed\n");
}

void test_correct_split_block() {
    void *ptr = custom_malloc(100);
    meta_data block = get_block_ptr(ptr);
    split_block(block, 50);
    assert(block->size == 50);
    assert(block->next->size == 50 - META_DATA_SIZE);
    custom_free(ptr);
    printf("test_correct_split_block passed\n");
}

void test_correct_merge_blocks() {
    void *ptr = custom_malloc(100);
    meta_data block = get_block_ptr(ptr);
    split_block(block, 50);

    custom_free(ptr);
    
    
    printf("test_correct_merge_blocks passed\n");
}

void test_is_valid_addr() {
    void *ptr = custom_malloc(100);
    assert(is_valid_addr(ptr) == 1);
    void *changed_ptr = ptr + 1;
    assert(is_valid_addr(changed_ptr) == 0);
    custom_free(ptr);
    printf("test_is_valid_addr passed\n");
}

int main() {
    printf("%lu\n", (unsigned long)META_DATA_SIZE);
 //   test_correct_split_block();
    test_correct_merge_blocks();
    
    return 0;
}