#include "../../utils.cpp"

void insertionSort(int* arr, int start, int end){
    if(start >= end) return;

    for(int i = start + 1; i <= end; ++i){
        int j = i;
        
        while(j > start && arr[j] < arr[j-1]){
            swap(arr,j,j-1);
            j--;
        }

    }
}

int main(){
    int n = 20;

    int arr[n] = {
        42, -17, 8, -93, 56,
        0, -4, 71, -28, 13,
        -65, 99, 24, -1, 37,
        -46, 82, -12, 5, -78
    };

    insertionSort(arr,0,n-1);

    printArr(arr,0,n);
}