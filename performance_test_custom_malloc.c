#include "./custom-alloc/custom_alloc.c"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ALLOCATIONS 10000
#define MAX_SIZE 4096 // Maximum size for an allocation (in bytes)
#define ALLOC_PROB 70 // Probability (in %) of allocating memory
#define FREE_PROB 30  // Probability (in %) of freeing memory

void test_malloc_with_random_sizes() {
    void* pointers[MAX_ALLOCATIONS] = {NULL};
    int allocated_count = 0;

    srand(time(NULL)); // Seed for random number generation

    for (int i = 0; i < MAX_ALLOCATIONS; i++) {
        int action = rand() % 100; // Generate a random number between 0 and 99

        if (action < ALLOC_PROB && allocated_count < MAX_ALLOCATIONS) {
            // Allocate memory of random size between 1 and MAX_SIZE bytes
            size_t size = (rand() % MAX_SIZE) + 1;
            pointers[allocated_count] = custom_malloc(size);

            if (pointers[allocated_count] == NULL) {
                perror("malloc failed");
                exit(EXIT_FAILURE);
            }

            //printf("Allocated %zu bytes at index %d\n", size, allocated_count);
            allocated_count++;
        } else if (action < ALLOC_PROB + FREE_PROB && allocated_count > 0) {
            // Free a random pointer
            int free_index = rand() % allocated_count;

            if (pointers[free_index] != NULL) {
               
            
                custom_free(pointers[free_index]);
                
                //printf("Freed memory at index %d\n", free_index);
                pointers[free_index] = NULL;
            }
        }
    }

    // Free any remaining allocated memory
    for (int i = 0; i < allocated_count; i++) {
        if (pointers[i] != NULL) {
            custom_free(pointers[i]);
            //printf("Freed remaining memory at index %d\n", i);
            pointers[i] = NULL;
        }
    }

    //printf("All memory has been freed.\n");
}

int main(void) {
    test_malloc_with_random_sizes();
    return 0;
}