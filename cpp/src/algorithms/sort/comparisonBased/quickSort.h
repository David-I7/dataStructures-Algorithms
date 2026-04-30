#include "bubbleSort.h"
#pragma once

template <typename T>
int hoarePartition(T* list,int l, int r,int(*comparator)(const T&,const T&)){
    int i = l+1, j = r;
    const T& pivot = list[l];

    while(true){

        while(i <= r && comparator(list[i], pivot) < 0) ++i;
        
        while(j > l && comparator(list[j], pivot) >= 0) --j;
        
        if(i > j) break;

        swap(list, i, j);
        
        ++i;--j;
    }
    
    if (j != l){
        swap(list, l, j);
    }

    return j;
}

template <typename T>
void quickSort(T* list,int l, int r,int k,int(*comparator)(const T&,const T&)){
    if (l >= r) return;
    else if (r-l < k){
        bubbleSort(list,l,r,comparator);
        return;
    }

    int partitionIndex = hoarePartition(list, l, r,comparator);
    quickSort(list, l, partitionIndex-1,k,comparator);
    quickSort(list, partitionIndex + 1, r,k,comparator);
}
    
