#include <fstream>
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
    const T temp = list[i];
    list[i] = list[j];
    list[j] = temp;
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
    if(a < b) return -1;
    if(a > b) return 1;
    return 0;
}

template <typename T>
int sortDescending(const T& a,const T& b){
    if(a < b) return 1;
    if(a > b) return -1;
    return 0;
}

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

template <typename T>
void merge(T*list,int l,int mid,int r,int(*comparator)(const T&,const T&)){
    int lenA = (mid-l) + 1,lenB = (r-mid);
    T a[lenA], b[lenB];

    for(int i = 0; i < lenA;++i)
        a[i] = list[l+i];
    
    for(int i = 0; i < lenB;++i)
        b[i] = list[i+(mid+1)];

    int i=0, j=0, k=l;
    while(i < lenA && j < lenB){
        if (comparator(a[i],b[j]) <= 0){
            list[k++] = a[i++];
        }else{
            list[k++] = b[j++];
        }
    }
    
    while(i < lenA)
        list[k++] = a[i++];
    
    while(j < lenB)
        list[k++] = b[j++];
    
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

int main(){
    std::ifstream fis("sort.txt");

    if(!fis.is_open()){
        std::cout << "Fiserul nu s-a putut deschide!\n";
        return 1;
    }

    int n,k,i=0;
    
    fis >> n >> k;

    if(n < 0 || k > n){
        std::cout << "Datele nu au fost introduse correct!\n";
        return 1;
    }
    
    int list[n];

    i = 0;
    while(i < n && fis >> list[i]){
        ++i;
    }

    if(i != n){
        std::cout << "Nu au fost introduse n elemente in fisier!\n";
        return 1;
    }

    mergeSort(list, 0, n-1, k, sortAscending);
    printList(list, n);
    std::cout << "\n\n";

    mergeSort(list, 0, n-1, k, sortDescending);
    printList(list, n);
    std::cout << "\n\n";

    quickSort(list, 0, n-1, k, sortAscending);
    printList(list, n);
    std::cout << "\n\n";

    quickSort(list, 0, n-1, k, sortDescending);
    printList(list, n);
    std::cout << "\n\n";
}