#include "../../utils.cpp"


void bubbleSort(int* arr, int start, int end){
    if(start >= end) return;

    for(int i = start; i < end; i++){
        bool changed = false;
        for(int j = start; j < end - (start - i); j++){
            if(arr[j] > arr[j+1]){
                swap(arr,j,j+1);
                changed = true;
            }
        }

        if(!changed) break;
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

    bubbleSort(arr,1,2);

    printArr(arr,0,n);
}