#include "./custom-alloc/custom_alloc.c"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ALLOCATIONS 10000
#define MAX_SIZE 4096 // Maximum size for an allocation (in bytes)
#define ALLOC_PROB 70 // Probability (in %) of allocating memory
#define FREE_PROB 30  // Probability (in %) of freeing memory

meta_data best_fit(meta_data *prev, size_t size)
{
   // implement best fit

   meta_data current = heap_head;
   meta_data best_fit_ptr = NULL;
   size_t min_size = -1;

   while (current)
   {
       if (current->free && current->size >= size && current->size < min_size)
       {
           best_fit_ptr = current;
           min_size = current->size;
           *prev = current;
       }
       current = current->next;
   }

    return best_fit_ptr;

}


meta_data next_fit(meta_data *prev, size_t size)
{
   static meta_data last_allocated = NULL; //static pointer to track the last allocated block
   //if heap_head is NULL, no memory blocks exist
   if (!heap_head){
      return NULL;
   } 

   //start searching from the last allocated block or the beginning of the heap
   meta_data current;
   if (last_allocated) {
      current = last_allocated;
   } else {
      current = heap_head;
   }
   *prev = NULL;

   //search the linked list of blocks in a circular fashion
   while (1) {
      if (current->free && current->size >= size) {
         last_allocated = current;  
         return current;
      }

      //update prev and move to the next block
      *prev = current;
      current = current->next;

      //if we reach the end, wrap around to the beginning of the heap
      if (!current){
         current = heap_head;
      } 

      //stop searching if we return to the starting point
      meta_data start_point;

      if (last_allocated) {
         start_point = last_allocated;
      } else {
         start_point = heap_head;
      }
      if (current == start_point) {
         break;
      }
   }
   return NULL; //no suitable free block found
}


meta_data first_fit(meta_data *prev, size_t size)
{
    // implement first fit algorithm

    meta_data current = heap_head;
    while (current && !(current->free && current->size >= size))
    {
        *prev = current;
        current = current->next;
    }

    return current;

}

size_t get_heap_size() {
    // Use sbrk(0) to get the current program break
    void *current_break = sbrk(0);

    // The initial program break represents the base of the heap
    static void *heap_base = NULL;

    // Initialize the heap base if this is the first call
    if (heap_base == NULL) {
        heap_base = current_break;
    }

    // Calculate the heap size as the difference between the current break and the base
    return (size_t)((char *)current_break - (char *)heap_base);
}


unsigned long* test_malloc(int max_allocations, meta_data (*find_free_block)(meta_data*, size_t)) {
    void* pointers[max_allocations];
    size_t heap_sizes[max_allocations];
    unsigned long allocation_duration[max_allocations];
    int allocated_count = 0;

    srand(time(NULL)); // Seed for random number generation
    unsigned long start_time = clock();
    for (int i = 0; i < max_allocations; i++) {
        int action = rand() % 100; // Generate a random number between 0 and 99

        if (action < ALLOC_PROB && allocated_count < max_allocations) {
            // Allocate memory of random size between 1 and MAX_SIZE bytes
            size_t size = (rand() % MAX_SIZE) + 1;
            pointers[allocated_count] = custom_malloc(size, find_free_block);
            pointers[allocated_count] = malloc(size);
    

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
                //free(pointers[free_index]);
                
                //printf("Freed memory at index %d\n", free_index);
                pointers[free_index] = NULL;
            }
        }
        unsigned long end_time = clock();
        allocation_duration[i] = end_time - start_time;
        heap_sizes[i] = get_heap_size();

    }
    

    // Free any remaining allocated memory
    for (int i = 0; i < allocated_count; i++) {
        if (pointers[i] != NULL) {
            custom_free(pointers[i]);
            //free(pointers[i]);
            //printf("Freed remaining memory at index %d\n", i);
            pointers[i] = NULL;
        }
    }

    // compute avarage allocation duration
    long total_duration = 0;
    for (int i = 0; i < max_allocations; i++) {
        total_duration += allocation_duration[i];
    }
    long average_duration = total_duration / max_allocations;

    // compute avarage heap size
    size_t total_heap_size = 0;
    for (int i = 0; i < max_allocations; i++) {
        total_heap_size += heap_sizes[i];
    }
    size_t average_heap_size = total_heap_size / max_allocations;

    //printf("All memory has been freed.\n");
    unsigned long* result = (unsigned long*)malloc(2 * sizeof(unsigned long));
    result[0] = average_duration;
    result[1] = average_heap_size;
    return result;
}

char* to_string(unsigned long* result) {
    char* str = (char*)malloc(100 * sizeof(char));
    sprintf(str, "%lu %lu\n", result[0], result[1]);
    return str;
}

int main(void) {
    FILE *fp = fopen("output.txt", "w");
    if (fp == NULL) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 9900; i < MAX_ALLOCATIONS; i++)
    {
        unsigned long* result1 = test_malloc(i, &first_fit);
        //unsigned long* result2 = test_malloc(MAX_ALLOCATIONS, &next_fit);
        //unsigned long* result3 = test_malloc(MAX_ALLOCATIONS, &best_fit);
        char* str1 = to_string(result1);
        //char* str2 = to_string(result2);

        fwrite(str1, strlen(str1),1, fp);

        
        //write(fp, result2, sizeof(unsigned long) * 2);
        //write(fp, result3, sizeof(unsigned long) * 2);


        printf("%d First fit: Average allocation duration: %lu, Average heap size: %lu\n",i ,result1[0], result1[1]);
        //printf("Next fit: Average allocation duration: %lu, Average heap size: %lu\n", result2[0], result2[1]);
        //printf("Best fit: Average allocation duration: %lu, Average heap size: %lu\n", result3[0], result3[1]);

        free(result1);
        //free(result2);
        //free(result3);
    }

    fclose(fp);

   


   

    return 0;
}