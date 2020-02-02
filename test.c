#include <stdio.h>
#include <stdlib.h>

int main()
{
    int* test = (int*)malloc(sizeof(int)*5);
    test[0] = 1;

    printf("%d\n", test[0]);    

    return 0;
}