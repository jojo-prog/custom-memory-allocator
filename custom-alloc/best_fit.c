#include "custom_alloc.h"
meta_data find_free_block(meta_data *prev, size_t size)
{
   // implement best fit

   meta_data current = heap_head;
   meta_data best_fit = NULL;
   size_t min_size = -1;

   while (current)
   {
       if (current->free && current->size >= size && current->size < min_size)
       {
           best_fit = current;
           min_size = current->size;
           *prev = current;
       }
       current = current->next;
   }

}