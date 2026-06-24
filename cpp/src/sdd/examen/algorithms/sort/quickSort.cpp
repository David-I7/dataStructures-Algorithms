#include "../../utils.cpp"

int partition(int* arr, int start, int end){
   int i= start,j=start-1;
   int pivot = arr[end];

   for(;i<end;++i){
    if(arr[i] >= pivot) continue;
    swap(arr,++j,i);
   }

   if(++j != end){
    swap(arr,j,end);
   }

   return j;
}

void quickSort(int* arr, int start, int end){
    if(start >= end) return;

    int pivotIndex = partition(arr,start,end);
    quickSort(arr,start,pivotIndex -1);
    quickSort(arr,pivotIndex + 1, end);
}

int main(){
     int n = 20;

    int arr[n] = {
        42, -17, 8, -93, 56,
        0, -4, 71, -28, 13,
        -65, 99, 24, -1, 37,
        -46, 82, -12, 5, -78
    };

    quickSort(arr,0,n-1);

    printArr(arr,0,n);
}