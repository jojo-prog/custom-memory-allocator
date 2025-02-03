
#ifndef BUDDY_ALLOC_H
#define BUDDY_ALLOC_H
#include <cstddef>


typedef struct buddy_root_list buddy_root_list;
typedef struct buddy_node buddy_node;

struct buddy_root_list
{
    size_t list_size;
    buddy_root_list *next;
};

struct buddy_node
{
    size_t size;
    buddy_node *left_node;
    buddy_node *right_node;
    buddy_root_list *root;
    unsigned int free;
    void *ptr;
};

#define BUDDY_META_DATA_SIZE sizeof(struct buddy_node)
#define ROOT_LIST_SIZE sizeof(struct buddy_root_list)

#define PAGE_SIZE sysconf(_SC_PAGESIZE)
#define MEM_ALLOC_LOT_SIZE (1 * PAGE_SIZE)
#define MEM_DEALLOC_LOT_SIZE (2 * PAGE_SIZE)


buddy_root_list *buddy_list = NULL;

buddy_node *buddy_alloc(size_t size);


#endif // !BUDDY_ALLOC_H




