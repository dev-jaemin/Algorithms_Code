#include <stdio.h>
#include <stdlib.h>

int* ss(int arr[], int n);
int* merge(int* b, int* c, int n);

int main()
{
    int arr[] = {10, 20 ,15 ,18, 60, 16 , 75, 21};
    int b[] = {1, 5, 10, 15};
    int c[] = {4, 7, 14, 53};

    int* d = ss(arr, 8);

    for(int i=0;i<8;i++) printf("%d ", d[i]);
    return 0;
}

void ss(int* input, int n)
{
    //insertion sort
    /*for(int i=1;i<n-1;i++){
        int v = input[i];
        int j = i-1;

        while(j>=0 && input[j] > v){
            input[j+1] = input[j];
            j--;
        }

        input[j+1] = v;
    }*/

    //Merge Sort

    if(n <= 1) return;

    ss(input, n/2);
    ss(input+sizeof(int)*(n/2), n-n/2);
    
    merge(input, input+sizeof(int)*(n/2), n);
}

int* merge(int* b, int* c, int n)
{
    int i = 0;
    int j = 0;
    int k = 0;

    int* a = (int*)malloc(sizeof(int)*n);

    while (i < n/2 && j < n - n/2){
        if(b[i] < c[j]){
            a[k] = b[i];
            i++;
        }
        else{
            a[k] = c[j];
            j++;
        }
        k++;
    }

    if(i == n/2){
        while (j < n - n/2){
            a[k] = c[j];
            k++;
            j++;
        }
    }
    else{
        while (i < n/2){
            a[k] = b[i];
            k++;
            i++;
        }
    }

    return a;
}