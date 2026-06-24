#include "../../utils.cpp"

void selectionSort(int* arr, int start, int end){
    if(start >= end) return;

    for(int i = start; i < end; ++i){
        int minIndex = i;
        
        for(int j = i+1; j <= end; ++j){
            if(arr[minIndex] > arr[j]){
                minIndex = j;
            }
        }
        
        if(minIndex != i){
            swap(arr,i,minIndex);
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

    selectionSort(arr,2,3);

    printArr(arr,0,n);
}