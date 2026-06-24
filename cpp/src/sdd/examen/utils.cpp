#include <iostream>

void printArr(int* arr, int start, int end){
    if(start > end) return;
    if(start == end) {std::cout << "[]"; return;};

    std::cout << "[";

    int i = start;
    for(; i < end - 1; ++i){
        std::cout << arr[i] << ", ";
    }
    
    std::cout << arr[i] << "]";
}

void swap(int* arr, int i, int j){
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}