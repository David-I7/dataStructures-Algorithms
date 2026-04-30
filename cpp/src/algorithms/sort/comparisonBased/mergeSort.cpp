#include "mergeSort.h"

int main(){
    int n = 10;
    int k = 2;
    int arr[] = {10, -1, 2, -5, 0, -1, 43, 9, 99, 54};
    
    mergeSort(arr, 0, n-1,k, sortAscending);
    printList(arr, n);
    std::cout << "\n";
   
    mergeSort(arr, 0, n-1,k, sortDescending);
    printList(arr, n);
    std::cout << "\n";
    
    // only sort the elements at positions 1-3 
    mergeSort(arr, 1, 3,k, sortAscending);
    printList(arr, n);
    std::cout << "\n";

    char strs[] = {'a','c','d','b','a','e','g','p','o','z'};
    mergeSort(strs, 0, n-1,k, sortAscending);
    printList(strs, n);
    std::cout << "\n";
   
    mergeSort(strs, 0, n-1,k, sortDescending);
    printList(strs, n);
    std::cout << "\n";
    
    // only sort the elements at positions 1-3 
    mergeSort(strs, 1, 3,k, sortAscending);
    printList(strs, n);
    std::cout << "\n";
}