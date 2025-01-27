#include "../custom_alloc.c"

int main()
{
    int *a = custom_malloc(sizeof(int));
    custom_free(a);
    a[0] = 5;

    return 0;
}