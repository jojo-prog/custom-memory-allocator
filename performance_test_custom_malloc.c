#include "./custom-alloc/custom_alloc.c"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#define MAX_ALLOCATIONS 10000
#define MAX_SIZE 4096 // Maximum size for an allocation (in bytes)
#define ALLOC_PROB 70 // Probability (in %) of allocating memory
#define FREE_PROB 30  // Probability (in %) of freeing memory

size_t get_heap_size()
{
    // Use sbrk(0) to get the current program break
    void *current_break = sbrk(0);

    // The initial program break represents the base of the heap
    static void *heap_base = NULL;

    // Initialize the heap base if this is the first call
    if (heap_base == NULL)
    {
        heap_base = current_break;
    }

    // Calculate the heap size as the difference between the current break and the base
    return (size_t)((char *)current_break - (char *)heap_base);
}

unsigned long *test_malloc(int max_allocations, meta_data (*find_free_block)(meta_data *prev, size_t size))
{
    void *pointers[max_allocations];
    size_t heap_sizes[max_allocations];
    unsigned long allocation_duration[max_allocations];
    int allocated_count = 0;

    srand(1000); // Seed for random number generation
    unsigned long start_time = clock();
    for (int i = 0; i < max_allocations; i++)
    {
        int action = rand() % 100; // Generate a random number between 0 and 99
        // clear consol
        printf("Iteration: %d\n", i);
        if (action < ALLOC_PROB && allocated_count < max_allocations)
        {
            // Allocate memory of random size between 1 and MAX_SIZE bytes
            size_t size = (rand() % MAX_SIZE) + 1;
            pointers[allocated_count] = custom_malloc(size, find_free_block);
            // pointers[allocated_count] = malloc(size);

            meta_data current = mem_pool;
            printf("After allocation\n");
            while (current)
            {
                printf("Block size: %zu, Block free: %d, Next: %p, Prev: %p\n", current->size, current->free, current->next, current->prev);
                current = current->next;
            }

            printf("\n");

            if (pointers[allocated_count] == NULL)
            {
                perror("malloc failed");
                exit(EXIT_FAILURE);
            }

            // printf("Allocated %zu bytes at index %d\n", size, allocated_count);
            allocated_count++;
        }
        else if (action < ALLOC_PROB + FREE_PROB && allocated_count > 0)
        {
            // Free a random pointer
            int free_index = rand() % allocated_count;

            if (pointers[free_index] != NULL)
            {

                custom_free(pointers[free_index]);
                // free(pointers[free_index]);
                meta_data current = mem_pool;
                printf("After free\n");
                while (current)
                {

                    printf("Block size: %zu, Block free: %d, Next: %p, Prev: %p\n", current->size, current->free, current->next, current->prev);
                    current = current->next;
                }

                // printf("Freed memory at index %d\n", free_index);
                pointers[free_index] = NULL;
            }
        }
        unsigned long end_time = clock();
        allocation_duration[i] = end_time - start_time;
        heap_sizes[i] = get_heap_size();
    }

    // Free any remaining allocated memory
    for (int i = 0; i < allocated_count; i++)
    {
        if (pointers[i] != NULL)
        {
            custom_free(pointers[i]);
            // free(pointers[i]);
            // printf("Freed remaining memory at index %d\n", i);
            pointers[i] = NULL;
        }
    }

    printf("All memory has been freed.\n");
    meta_data current = mem_pool;
    while (current)
    {

        printf("Block size: %zu, Block free: %d, Next: %p, Prev: %p\n", current->size, current->free, current->next, current->prev);
        current = current->next;
    }

    // compute avarage allocation duration
    long total_duration = 0;
    for (int i = 0; i < max_allocations; i++)
    {
        total_duration += allocation_duration[i];
    }
    long average_duration = total_duration / max_allocations;

    // compute avarage heap size
    size_t total_heap_size = 0;
    for (int i = 0; i < max_allocations; i++)
    {
        total_heap_size += heap_sizes[i];
    }
    size_t average_heap_size = total_heap_size / max_allocations;

    // printf("All memory has been freed.\n");
    unsigned long *result = (unsigned long *)malloc(2 * sizeof(unsigned long));
    result[0] = average_duration;
    result[1] = average_heap_size;
    return result;
}

char *to_string(unsigned long *result)
{
    char *str = (char *)malloc(100 * sizeof(char));
    sprintf(str, "%lu %lu\n", result[0], result[1]);
    return str;
}

void run_test(char *filename, meta_data (*find_free_block)(meta_data *, size_t))
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < MAX_ALLOCATIONS; i++)
    {
        unsigned long *result = test_malloc(i, find_free_block);
        char *str1 = to_string(result);
        // char* str2 = to_string(result2);

        fwrite(str1, strlen(str1), 1, fp);

        // write(fp, result2, sizeof(unsigned long) * 2);
        // write(fp, result3, sizeof(unsigned long) * 2);

        printf("%d Average allocation duration: %lu, Average heap size: %lu, Splits: %lu, Frees: %lu\n", i, result[0], result[1], splits_count, frees_count);

        splits_count = 0;
        frees_count = 0;
        // printf("Next fit: Average allocation duration: %lu, Average heap size: %lu\n", result2[0], result2[1]);

        // free(result1);
        // free(result2);
        free(result);
    }

    fclose(fp);
}

int main(void)
{

    // test_next_fit();

    unsigned long *result = test_malloc(5000, &best_fit);
    printf("Average allocation duration: %lu, Average heap size: %lu, Splits: %lu, Frees: %lu, Merges: %lu\n", result[0], result[1], splits_count, frees_count, merges_count);
    printf("Possible heap shrink: %lu\n", possible_heap_shrink);

    // test_best_fit();

    return 0;
}