#include "../iterable/collection/list/List.h"
#include <functional>

template <typename E>
class Collections{
    public:
    enum SortStrategy{
        BUBBLE,
        MERGE,
        SHELL,
        SELECTION,
        BUCKET,
        QUICK,
        INSERTION
    };

    static void sort(List<E>* list,std::function<int(const E&,const E&)> comparator){
        bubbleSort(list, comparator);
    }

    static void sort(List<E>* list,std::function<int(const E&,const E&)> comparator,SortStrategy strategy){
        
        switch (strategy) {
            case BUBBLE:
                bubbleSort(list, comparator);
                break;
            case SELECTION:
                selectionSort(list, comparator);
                break;
            case INSERTION:
                selectionSort(list, comparator);
                break;
            default:
                return;
        }
    
    }

    private:
    static void bubbleSort(List<E>* list,std::function<int(const E&,const E&)> comparator){
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
    static void insertionSort(List<E>* list,std::function<int(const E&,const E&)> comparator){
        
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

    static void selectionSort(List<E>* list,std::function<int(const E&,const E&)> comparator){
        
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
};