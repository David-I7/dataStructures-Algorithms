#include <iostream>
#include <fstream>

using namespace std;

int hoarePartition(int* list,int l, int r){
    int i = l+1, j = r;
    int pivot = list[l];

    while(true){

        while(i <= r && list[i] < pivot) ++i;
        
        while(j > l && list[j] >= pivot) --j;
        
        if(i > j) break;

        int tmp = list[i];
        list[i] = list[j];
        list[j] = tmp; 
        
        ++i;--j;
    }
    
    if (j != l){
        list[l] = list[j];
        list[j] = pivot;
    }

    return j;
}

void _quickSort(int* list,int l, int r){
    if (l >= r) return;

    int partitionIndex = hoarePartition(list, l, r);
    _quickSort(list, l, partitionIndex-1);
    _quickSort(list, partitionIndex + 1, r);
}

void quickSort(int* list,int n){
    if (n<=1) return;
    _quickSort(list, 0, n - 1);
}
    
void printList(int* list, int n){
    if (n == 0){
        cout << "[]";
        return;
    }

    cout << "[";
    int i = 0;
    for(; i < n -1; ++i){
        cout << list[i] << ", ";
    }

    cout << list[i] << "]";
}

int main(){
    std::ifstream fis("data.txt");

    int n;
    fis >> n;
    int* list = new int[n];

    int i = 0;
    while(fis >> list[i++]);

    printList(list, n);
    cout << "\n";
    quickSort(list, n);
    printList(list, n);
    cout << "\n";

    delete[] list;
    return 0;
}