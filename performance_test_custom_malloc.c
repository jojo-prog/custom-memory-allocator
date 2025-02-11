#include "./custom-alloc/custom_alloc.c"
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#define MAX_ALLOCATIONS 10000
#define MAX_SIZE 4096 // Maximum size for an allocation (in bytes)
#define ALLOC_PROB 70 // Probability (in %) of allocating memory
#define FREE_PROB 30  // Probability (in %) of freeing memory
#define DEBUG 0
#define SEED (unsigned)time(NULL)

int stop_loop_index;
size_t r = 0;

void print_memory_pool()
{
    meta_data current = heap_list_start;
    while (current)
    {
        printf("Address: %p ,Block size: %zu, Block free: %d, Next: %p, Prev: %p\n",current ,current->size, current->free, current->next, current->prev);
        current = current->next;
    };
    sbrk(r);
    printf("\n");
    sbrk(-r);
}

#define DEBUG_PRINT \
    if (DEBUG)      \
    print_memory_pool()

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

unsigned long *test_malloc(int max_allocations, meta_data (*find_free_block)(meta_data *prev, size_t size), clock_t seed)
{
    if (max_allocations <= 0)
    {
        return (unsigned long[]){0, 0};
    }
    void *pointers[max_allocations];
    size_t heap_sizes[max_allocations];
    unsigned long allocation_duration[max_allocations];
    int allocated_count = 0;

    // srand(clock()); // Seed for random number generation
    //srand(0);
    srand(seed);
    //printf("Seed: %ld ", seed);
    unsigned long start_time = clock();
    for (int i = 0; i < max_allocations; i++)
    {
        int action = rand() % 100; // Generate a random number between 0 and 99
        size_t size = (rand() % (MAX_SIZE)) + 1;

        if (i == stop_loop_index && DEBUG)
        {
            printf("Stop loop index: %d\n", i);
            print_memory_pool();
        }

        if (action < ALLOC_PROB && allocated_count < max_allocations)
        {
            // Allocate memory of random size between 1 and MAX_SIZE bytes
            pointers[allocated_count] = find_free_block ? custom_malloc(size, find_free_block) : malloc(size);
        
            DEBUG_PRINT;

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
                
                find_free_block ? custom_free(pointers[free_index]) : free(pointers[free_index]);
                //print_memory_pool();
                DEBUG_PRINT;
                // free(pointers[free_index]);
                pointers[free_index] = NULL;
            }
        }

        unsigned long end_time = clock();
        allocation_duration[i] = end_time - start_time;
        heap_sizes[i] = get_heap_size();
    }

    // Free any remaining allocated memory
    int i;
    for (i = 0; i < allocated_count; i++)
    {
        if (pointers[i] != NULL)
        {
            find_free_block ? custom_free(pointers[i]) : free(pointers[i]);
            // free(pointers[i]);
            // printf("Freed remaining memory at index %d\n", i);
            pointers[i] = NULL;
            DEBUG_PRINT;
        }
    }
    DEBUG_PRINT;

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
    return (unsigned long[]){average_duration, average_heap_size};
}


void append_to_file(const char *filename, unsigned long v1, unsigned long v2) {
    char buffer[50];  // Fixed buffer size, enough for two unsigned long and a newline
    int length = snprintf(buffer, sizeof(buffer), "%lu %lu\n", v1, v2);
    
    if (length < 0 || (size_t ) length >= sizeof(buffer)) {
        return;  // Handle error if snprintf fails
    }

    FILE *file = fopen(filename, "a");  // Open file in append mode
    if (file) {
        fwrite(buffer, 1, length, file);  // Append the formatted string to the file
        fclose(file);
    }
}

void run_test(char *filename, meta_data (*find_free_block)(meta_data *, size_t))
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    unsigned long results[MAX_ALLOCATIONS][2];
    clock_t seed = clock();
    for (int i = 0; i < MAX_ALLOCATIONS; i++)
    {
        unsigned long *result = test_malloc(i, find_free_block, seed);
        results[i][0] = result[0];
        results[i][1] = result[1];

        printf("%d Average allocation duration: %lu, Average heap size: %lu\n", i, result[0], result[1]);
    }
    for (int i = 0; i < MAX_ALLOCATIONS; i++)
    {
       unsigned long *result = results[i];
       append_to_file(filename, result[0], result[1]);
    }


    fclose(fp);
}

void test_best_fit(int size, clock_t seed, int stop_index)
{
    stop_loop_index = stop_index;
    unsigned long *result = test_malloc(size, &best_fit, seed);
    printf("Best Fit: Average allocation duration: %lu, Average heap size: %lu\n", result[0], result[1]);
}

void test_next_fit(int size, clock_t seed, int stop_index)
{
    stop_loop_index = stop_index;
    unsigned long *result = test_malloc(size, &next_fit, seed);
    printf("Next Fit: Average allocation duration: %lu, Average heap size: %lu\n", result[0], result[1]);
}

void test_first_fit(int size, clock_t seed, int stop_index)
{
    stop_loop_index = stop_index;
    unsigned long *result = test_malloc(size, &first_fit, seed);
    printf("First Fit: Average allocation duration: %lu, Average heap size: %lu\n", result[0], result[1]);
}



int main(void)
{
    run_test("best_fit.txt", &best_fit);
    //run_test("next_fit.txt", &next_fit);
    //run_test("first_fit.txt", &first_fit);
    // test_next_fit();
    void* p1 = custom_malloc(100, &next_fit);

}