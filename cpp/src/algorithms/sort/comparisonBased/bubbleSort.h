#pragma once
#include <iostream>
#include <ostream>

template <typename T>
void printList(T* list,int n){
    if (n <= 0) {
        std::cout << "[]"; 
        return;
    }

    std::cout << "["; 
    
    for(int i = 0; i < n -1; ++i){
        std::cout << list[i] << ", "; 
    }

    std::cout << list[n-1] << "]"; 
}

template <typename T>
void swap(T* list,int i, int j){
    const T temp = std::move(list[i]);
    list[i] = std::move(list[j]);
    list[j] = std::move(temp);
}

template <typename T>
void bubbleSort(T* list,int l, int r, int(*comparator)(const T&,const T&)){

    for(int i = l; i < r;++i){
        bool hasChanged = false;
        
        for(int j = l; j < r - (i-l);++j){
            if (comparator(list[j],list[j + 1]) > 0){
                swap(list,j,j+1);
                hasChanged = true;
            }
        }

        if(!hasChanged) break;
    }
}

template <typename T>
int sortAscending(const T& a,const T& b){
    return a-b;
}

template <typename T>
int sortDescending(const T& a,const T& b){
    return b-a;
}