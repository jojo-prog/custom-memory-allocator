#include "custom_alloc.h"

meta_data last_allocated = NULL; //static pointer to track the last allocated block


//This function searches for a free memory block starting from the last allocated block (`last_allocated`) 
//and continues the search in a circular manner through the linked list of memory blocks.
meta_data find_free_block(meta_data *prev, size_t size)
{
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
