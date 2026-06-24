#include "../../utils.cpp"


void merge(int* arr, int start, int mid, int end){
    int lenA = mid - start + 1;
    int lenB = end - mid;
    int A[lenA], B[lenB];

    for(int i = start; i <= mid; ++i){
        A[i -start] = arr[i];
    }

     for(int i = mid+1; i <= end; ++i){
        B[i - (mid+1)] = arr[i];
    }

    int i=0,j=0,k=start;
    while(i < lenA && j < lenB){
        if(A[i] < B[j]){
            arr[k++] = A[i++];
        }else{
            arr[k++] = B[j++];
        }
    }

    while(i < lenA){
        arr[k++] = A[i++];
    }

    while(j < lenB){
        arr[k++] = B[j++];
    }
}

void mergeSort(int* arr, int start, int end){
    if(start >= end) return;

    int mid = (start + end) / 2;
    mergeSort(arr,start,mid);
    mergeSort(arr,mid+1,end);

    merge(arr,start,mid,end);
}


int main(){

    int n = 20;
    
    int arr[n] = {
    42, -17, 8, -93, 56,
    0, -4, 71, -28, 13,
    -65, 99, 24, -1, 37,
    -46, 82, -12, 5, -78
};

    mergeSort(arr,2,3);

    printArr(arr,0,n);
}