#include "../custom_alloc.c"
#include "../custom_mem_management.c"

int main()
{
    int *a = custom_malloc(sizeof(int));
    custom_free(a);
    a[0] = 5;

    return 0;
}