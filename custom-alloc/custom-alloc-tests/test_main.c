#include "../custom_alloc.c"
#include "../custom_mem_management.c"

int main()
{
    int *a = custom_malloc(10 * sizeof(int));
    int *b = custom_malloc(10 * sizeof(int));
    int *c = custom_malloc(10 * sizeof(int));
    int *d = custom_malloc(10 * sizeof(int));

    custom_free(a);
    custom_free(b);
    custom_free(c);
    custom_free(d);

    return 0;
}