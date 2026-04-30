#include "bubbleSort.h"

int main(){
    int n = 10;
    int arr[] = {10, -1, 2, -5, 0, -1, 43, 9, 99, 54};
    
    bubbleSort(arr, 0, n-1, sortAscending);
    printList(arr, n);
    std::cout << "\n";
   
    bubbleSort(arr, 0, n-1, sortDescending);
    printList(arr, n);
    std::cout << "\n";
    
    // only sort the elements at positions 1-3 
    bubbleSort(arr, 1, 3, sortAscending);
    printList(arr, n);
    std::cout << "\n";

    char strs[] = {'a','c','d','b','a','e','g','p','o','z'};
    bubbleSort(strs, 0, n-1, sortAscending);
    printList(strs, n);
    std::cout << "\n";
   
    bubbleSort(strs, 0, n-1, sortDescending);
    printList(strs, n);
    std::cout << "\n";
    
    // only sort the elements at positions 1-3 
    bubbleSort(strs, 1, 3, sortAscending);
    printList(strs, n);
    std::cout << "\n";
}