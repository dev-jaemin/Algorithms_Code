#include <stdio.h>

int LomutoPartition(int* arr, int n)
{
    int p = arr[0];
    int s = 0;
    for(int i=1;i<n;i++){
        if(p > arr[i]){
            s++;
            int tmp = arr[i];
            arr[i] = arr[s];
            arr[s] = tmp;
        }
    }
    int tmp = arr[s];
    arr[s] = p;
    arr[0] = tmp;

    return s;
}

int QuickSelect(int* arr, int n, int key)
{
    int s = LomutoPartition(arr, n);
    if(s == key-1)
        return arr[key-1];
    else if(s > key - 1){
        return QuickSelect(arr, s, key);
    }
    else{
        return QuickSelect(&arr[s+1], n-s-1, key-s-1);
    }
}


int main()
{
    int arr[8] = {11, 2, 34, 325, 10, 6, 7, 30};

    printf("%d\n", QuickSelect(arr, 8, 2));
    
    return 0;
}