#include "../../utils.cpp"

void countingSort(int* arr, int start, int end){
    if(start >= end) return;

    int min = ~(1 << 31);
    int max = 1 << 31;

    for(int i = start; i <= end; ++i){
        if(arr[i] < min) min = arr[i];
        if(arr[i] > max) max = arr[i];
    }

    int k = (max - min) +  1;
    int freq[k]{};

    for(int i = start; i <=end; ++i ){
        freq[arr[i] - min] += 1; 
    }

    for(int i = 1; i < k; ++i){
        freq[i] = freq[i-1] + freq[i];
    }

    for(int i = k-1; i>0; i--){
        freq[i] = freq[i-1];
    }

    freq[0] = 0;

    int n = (end - start)+1;
    int res[n];

    for(int i = start; i <= end; ++i){
        res[freq[arr[i] - min]++] = arr[i];
    }

    print(res,0,n);
}

int main(){
    int n = 20;

    int arr[n] = {
        42, -17, 8, -93, 56,
        0, -4, 71, -28, 13,
        -65, 99, 24, -1, 37,
        -46, 82, -12,8, -78
    };

    countingSort(arr,0,n-1);
}