#include "custom_alloc.h"
meta_data find_free_block(meta_data *prev, size_t size)
{
   
   static meta_data last = NULL;
   if (!last) {
      meta_data current = heap_head;
      while (current && !(current->free && current->size >= size))
      {
         *prev = current;
         current = current->next;
      }
      last = current;
   } else {
      while (last && !(last->free && last->size >= size))
      {
         *prev = last;
         last = last->next;
      }
   }
   

   

   return last;
}