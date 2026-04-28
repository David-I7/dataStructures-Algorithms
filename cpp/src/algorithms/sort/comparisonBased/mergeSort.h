#include "bubbleSort.h"
#pragma once

template <typename T>
void merge(T*list,int l,int mid,int r,int(*comparator)(const T&,const T&)){
    int lenA = (mid-l) + 1,lenB = (r-mid);
    T a[lenA], b[lenB];

    for(int i = 0; i < lenA;++i){
        a[i] = std::move(list[l+i]);
    }
    for(int i = 0; i < lenB;++i){
        b[i] = std::move(list[i+(mid+1)]);
    }

    int i = 0,j=0,k=l;
    while(i < lenA && j < lenB){
        if (comparator(a[i],b[j]) <= 0){
            list[k++] = std::move(a[i++]);
        }else{
            list[k++] = std::move(b[j++]);
        }
    }
    while(i < lenA){
        list[k++] = std::move(a[i++]);
    }
    while(i < lenB){
        list[k++] = std::move(b[j++]);
    }
}

template <typename T>
void mergeSort(T*list,int l,int r,int k,int(*comparator)(const T&,const T&)){
    if (r - l < 1) return;
    else if(r - l < k){
        bubbleSort(list, l, r, comparator);
        return;
    }

    int mid = (r+l) /2;
    mergeSort(list, l, mid,  k, comparator);
    mergeSort(list, mid+1, r,  k, comparator);

    merge(list,l,mid,r,comparator);
}

