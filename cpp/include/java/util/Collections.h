#include "../iterable/collection/list/List.h"
#include <cstddef>
#include <cstdlib>
#include <functional>

template <typename E>
class Collections{
    public:
    enum SortStrategy{
        BUBBLE,
        MERGE,
        SHELL,
        SELECTION,
        QUICK,
        INSERTION
    };

    static void sort(List<E>* list,const std::function<int(const E&,const E&)>& comparator){
        if (list->size() < 100){
            insertionSort(list, comparator);
        }else{
            quickSort(list, comparator);
        }
    }

    static void sort(List<E>* list,const std::function<int(const E&,const E&)>& comparator,SortStrategy strategy){
        switch (strategy) {
            case BUBBLE:
                bubbleSort(list, comparator);
                break;
            case SELECTION:
                selectionSort(list, comparator);
                break;
            case INSERTION:
                insertionSort(list, comparator);
                break;
            case MERGE:
                mergeSort(list, comparator);
                break;
            case SHELL:
                shellSort(list, comparator);
                break;
            case QUICK:
                quickSort(list, comparator);
                break;
            default:
                return;
        }
    }

    private:
    static void bubbleSort(List<E>* list,const std::function<int(const E&,const E&)>& comparator){
        bool hasChanged = false;
        for(int i = 0; i < list->size()-1;++i){
            hasChanged = false;
            for(int j =i+1; j < list->size() ;++j){
                if(comparator(list->get(i),list->get(j)) > 0){
                    hasChanged = true;
                    E tmp = list->get(i);
                    list->set(i,list->get(j));
                    list->set(j,tmp);
                }
            }
            // list is already sorted
            if (!hasChanged) break;
        }
    }

    static void insertionSort(List<E>* list,const std::function<int(const E&,const E&)>& comparator){
        for(int i=1; i < list->size(); ++i){
            int j = i;
            while(j > 0 && comparator(list->get(j),list->get(j -1)) < 0){
                E tmp = list->get(j);
                list->set(j,list->get(j-1));
                list->set(j-1,tmp);
                --j;
            }
        }
    }

    static void selectionSort(List<E>* list,const std::function<int(const E&,const E&)>& comparator){
        for(int i=0; i < list->size() - 1; ++i){
            int minIdx = i;
            for(int j = i+1;j< list->size();++j){
                if (comparator(list->get(minIdx),list->get(j)) > 0){
                    minIdx = j;
                }
            }
            if (minIdx != i){
                E tmp = list->get(minIdx);
                list->set(minIdx,list->get(i));
                list->set(i,tmp);
            }
        }
    }

    static void shellSort(List<E>* list,const std::function<int(const E&,const E&)>& comparator){
        std::size_t gap = list->size() / 2;
        
        while(gap > 0){
            for(std::size_t i = gap; i< list->size();++i){
                std::size_t j = i;
                
                while(j >= gap && comparator(list->get(j),list->get(j-gap)) < 0){
                    E tmp = list->get(j);
                    list->set(j,list->get(j-gap));
                    list->set(j-gap,tmp);

                    j-=gap;
                }
            }

            gap = gap / 2;
        }
    }

    static void quickSort(List<E>* list,const std::function<int(const E&,const E&)>& comparator){
        if (list->size() <=1) return;
        _quickSort(list, comparator, 0, list->size() - 1);
    }
    
    static void _quickSort(List<E>* list,const std::function<int(const E&,const E&)>& comparator,int l, int r){
        if (r - l <= 0) return;

        std::size_t partitionIndex = _hoarePartition(list,comparator, l, r);
        _quickSort(list, comparator, l, partitionIndex-1);
        _quickSort(list, comparator, partitionIndex + 1, r);
    }

    static std::size_t _hoarePartition(List<E>* list,const std::function<int(const E&,const E&)>& comparator,int l, int r){
        int i = l+1, j = r;
        E pivot = list->get(l);

        while(true){

            while(i <= r && comparator(list->get(i),pivot) <= 0) ++i;
            
            while(j > l && comparator(list->get(j),pivot) > 0) --j;
            
            if(i > j) break;

            E tmp = list->get(i);
            list->set(i,list->get(j));
            list->set(j,tmp);
            
            ++i;++j;
        }
        
        if (j != l){
            list->set(l,list->get(j));
            list->set(j,pivot);
        }

        return j;
    }

    static std::size_t _lomutoPartition(List<E>* list,const std::function<int(const E&,const E&)>& comparator,int l, int r){
        int i = 0, j = -1;
        E& pivot = list->get(r);

        for(i; i < r;++i){
            if (comparator(list->get(i),pivot) <= 0){
                ++j;
                if (i != j){
                    E tmp = list->get(i);
                    list->set(i,list->get(j));
                    list->set(j,tmp);
                }
            }
        }

        ++j;
        if (r != j){
            E tmp = list->get(r);
            list->set(r,list->get(j));
            list->set(j,tmp);
        }

        return j;
    }

    static void mergeSort(List<E>* list,const std::function<int(const E&,const E&)>& comparator){
        if (list->size() <= 1) return;
        _mergeSort(list, comparator, 0, list->size() -1);
    }

    static void _mergeSort(List<E>* list,const std::function<int(const E&,const E&)>& comparator,int l, int r){
        if( r - l == 0) return;

        std::size_t mid = (r+l) / 2;

        _mergeSort(list, comparator, l, mid);
        _mergeSort(list, comparator, mid+1, r);

        _merge(list,comparator,l,mid,r);
    }

    static void _merge(List<E>* list,const std::function<int(const E&,const E&)>& comparator,int l, int mid, int r){
        List<E>* left = list->subList(l, mid+1);
        List<E>* right = list->subList(mid+1, r+1);

        int i,j,k;
        i=j=0;
        k=l;

        while(i < left->size() && j < right->size()){
            if (comparator(left->get(i),right->get(j)) <=0){
                list->set(k,left->get(i));
                ++k;++i;
            }else{
                list->set(k,right->get(j));
                ++k;++j;
            }
        }
        while(i < left->size()){
            list->set(k,left->get(i));
            ++k;++i;
        }
        while(i < right->size()){
            list->set(k,right->get(i));
            ++k;++i;
        }

        delete left;
        delete right;
    }


};