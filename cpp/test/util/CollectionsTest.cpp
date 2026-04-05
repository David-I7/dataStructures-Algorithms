#include <gtest/gtest.h>
#include <iostream>
#include "java/iterable/collection/list/arrayList/ArrayList.h"
#include "java/util/Collections.h"

namespace{
    template<typename T> 
    ArrayList<T>* setupArrayList(int capacity, std::function<T(int)> mapper){
        ArrayList<T>* arr = new ArrayList<T>(capacity);
        for(int i = 0 ; i < capacity; ++i){
            arr->add(mapper(i));
        }
        return arr;
    }

    template<typename T> 
    ArrayList<T>* setupMixedArrayList(int capacity,std::function<T(int)> mapper){
        ArrayList<T>* arr = new ArrayList<T>(capacity);
        
        for(int i = 0; i < capacity;++i){
            arr->add(mapper(i));
        }
        
        for(int i = 0 ; i < capacity / 2; ++i){
            T tmp = arr->get(i);
            arr->set(i,arr->get(capacity - 1 - i));
            arr->set(capacity - 1 - i,tmp);
        }

        return arr;
    }

     int descendingMapper(int index){return -index;}
     int ascendingMapper(int index){return index;}


    TEST(Collections,BubbleSortSortsItemsInAscendingOrder){
        ArrayList<int>*arr = setupArrayList<int>(10, descendingMapper);
        ArrayList<int>*arr2 = setupMixedArrayList<int>(21, descendingMapper);
        
        Collections<int>::sort(arr,[](int a, int b){ return a-b;},Collections<int>::SortStrategy::BUBBLE);
        Collections<int>::sort(arr2,[](int a, int b){ return a-b;},Collections<int>::SortStrategy::BUBBLE);
        
        ArrayList<int>::ArrayListIterator* it = arr->iterator();
        
        int num = -9;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num++);
        }

        delete it;

        it = arr2->iterator();
        num = -20;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num++);
        }

        delete it;
    }

    TEST(Collections,BubbleSortSortsItemsInDescendingOrder){
        ArrayList<int>*arr = setupArrayList<int>(10, ascendingMapper);
        ArrayList<int>*arr2 = setupArrayList<int>(21, ascendingMapper);
        
        Collections<int>::sort(arr,[](int a, int b){ return b-a;},Collections<int>::SortStrategy::BUBBLE);
        Collections<int>::sort(arr2,[](int a, int b){ return b-a;},Collections<int>::SortStrategy::BUBBLE);
        
        ArrayList<int>::ArrayListIterator* it = arr->iterator();
        
        int num = 9;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num--);
        }

        delete it;

        it = arr2->iterator();
        num = 20;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num--);
        }

        delete it;
    }

    TEST(Collections,InsertionSortSortsItemsInAscendingOrder){
         ArrayList<int>*arr = setupArrayList<int>(10, descendingMapper);
        ArrayList<int>*arr2 = setupMixedArrayList<int>(21, descendingMapper);
        
        Collections<int>::sort(arr,[](int a, int b){ return a-b;},Collections<int>::SortStrategy::INSERTION);
        Collections<int>::sort(arr2,[](int a, int b){ return a-b;},Collections<int>::SortStrategy::INSERTION);
        
        ArrayList<int>::ArrayListIterator* it = arr->iterator();
   
        int num = -9;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num++);
        }

        delete it;

        it = arr2->iterator();
        num = -20;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num++);
        }

        delete it;
    }

    TEST(Collections,InsertionSortSortsItemsInDescendingOrder){
        ArrayList<int>*arr = setupArrayList<int>(10, ascendingMapper);
        ArrayList<int>*arr2 = setupArrayList<int>(21, ascendingMapper);
        
        Collections<int>::sort(arr,[](int a, int b){ return b-a;},Collections<int>::SortStrategy::INSERTION);
        Collections<int>::sort(arr2,[](int a, int b){ return b-a;},Collections<int>::SortStrategy::INSERTION);
        
        ArrayList<int>::ArrayListIterator* it = arr->iterator();
       
        int num = 9;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num--);
        }

        delete it;

        it = arr2->iterator();
        num = 20;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num--);
        }

        delete it;
    }

    TEST(Collections,SelectionSortSortsItemsInAscendingOrder){
         ArrayList<int>*arr = setupArrayList<int>(10, descendingMapper);
        ArrayList<int>*arr2 = setupMixedArrayList<int>(21, descendingMapper);
        
        Collections<int>::sort(arr,[](int a, int b){ return a-b;},Collections<int>::SortStrategy::SELECTION);
        Collections<int>::sort(arr2,[](int a, int b){ return a-b;},Collections<int>::SortStrategy::SELECTION);
        
        ArrayList<int>::ArrayListIterator* it = arr->iterator();
       
        int num = -9;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num++);
        }

        delete it;

        it = arr2->iterator();
        num = -20;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num++);
        }

        delete it;
    }

    TEST(Collections,SelectionSortSortsItemsInDescendingOrder){
        ArrayList<int>*arr = setupArrayList<int>(10, ascendingMapper);
        ArrayList<int>*arr2 = setupArrayList<int>(21, ascendingMapper);
        
        Collections<int>::sort(arr,[](int a, int b){ return b-a;},Collections<int>::SortStrategy::SELECTION);
        Collections<int>::sort(arr2,[](int a, int b){ return b-a;},Collections<int>::SortStrategy::SELECTION);
        
        ArrayList<int>::ArrayListIterator* it = arr->iterator();
       
        int num = 9;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num--);
        }

        delete it;

        it = arr2->iterator();
        num = 20;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num--);
        }

        delete it;
    }
   
    TEST(Collections,MergeSortSortsItemsInAscendingOrder){
        ArrayList<int>*arr = setupArrayList<int>(10, descendingMapper);
        ArrayList<int>*arr2 = setupMixedArrayList<int>(21, descendingMapper);
        
        Collections<int>::sort(arr,[](int a, int b){ return a-b;},Collections<int>::SortStrategy::MERGE);
        Collections<int>::sort(arr2,[](int a, int b){ return a-b;},Collections<int>::SortStrategy::MERGE);
        
        ArrayList<int>::ArrayListIterator* it = arr->iterator();
       
        int num = -9;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num++);
        }

        delete it;

        it = arr2->iterator();
        num = -20;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num++);
        }

        delete it;
    }

    TEST(Collections,MergeSortSortsItemsInDescendingOrder){
        ArrayList<int>*arr = setupArrayList<int>(10, ascendingMapper);
        ArrayList<int>*arr2 = setupArrayList<int>(21, ascendingMapper);
        
        Collections<int>::sort(arr,[](int a, int b){ return b-a;},Collections<int>::SortStrategy::MERGE);
        Collections<int>::sort(arr2,[](int a, int b){ return b-a;},Collections<int>::SortStrategy::MERGE);
        
        ArrayList<int>::ArrayListIterator* it = arr->iterator();
       
        int num = 9;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num--);
        }

        delete it;

        it = arr2->iterator();
        num = 20;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num--);
        }

        delete it;
    }
    
    TEST(Collections,ShellSortSortsItemsInAscendingOrder){
        ArrayList<int>*arr = setupArrayList<int>(10, descendingMapper);
        ArrayList<int>*arr2 = setupMixedArrayList<int>(21, descendingMapper);
        
        Collections<int>::sort(arr,[](int a, int b){ return a-b;},Collections<int>::SortStrategy::SHELL);
        Collections<int>::sort(arr2,[](int a, int b){ return a-b;},Collections<int>::SortStrategy::SHELL);
        
        ArrayList<int>::ArrayListIterator* it = arr->iterator();
       
        int num = -9;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num++);
        }

        delete it;

        it = arr2->iterator();
        num = -20;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num++);
        }

        delete it;
    }

    TEST(Collections,ShellSortSortsItemsInDescendingOrder){
        ArrayList<int>*arr = setupArrayList<int>(10, ascendingMapper);
        ArrayList<int>*arr2 = setupArrayList<int>(21, ascendingMapper);
        
        Collections<int>::sort(arr,[](int a, int b){ return b-a;},Collections<int>::SortStrategy::SHELL);
        Collections<int>::sort(arr2,[](int a, int b){ return b-a;},Collections<int>::SortStrategy::SHELL);
        
        ArrayList<int>::ArrayListIterator* it = arr->iterator();
       
        int num = 9;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num--);
        }

        delete it;

        it = arr2->iterator();
        num = 20;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num--);
        }

        delete it;
    }
    
    TEST(Collections,QuickSortSortsItemsInAscendingOrder){
        ArrayList<int>*arr = setupArrayList<int>(10, descendingMapper);
        ArrayList<int>*arr2 = setupMixedArrayList<int>(21, descendingMapper);
        
        Collections<int>::sort(arr,[](int a, int b){ return a-b;},Collections<int>::SortStrategy::QUICK);
        Collections<int>::sort(arr2,[](int a, int b){ return a-b;},Collections<int>::SortStrategy::QUICK);
        
        ArrayList<int>::ArrayListIterator* it = arr->iterator();
       
        int num = -9;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num++);
        }

        delete it;

        it = arr2->iterator();
        num = -20;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num++);
        }

        delete it;
    }

    TEST(Collections,QuickSortSortsItemsInDescendingOrder){
        ArrayList<int>*arr = setupArrayList<int>(10, ascendingMapper);
        ArrayList<int>*arr2 = setupArrayList<int>(21, ascendingMapper);
        
        Collections<int>::sort(arr,[](int a, int b){ return b-a;},Collections<int>::SortStrategy::QUICK);
        Collections<int>::sort(arr2,[](int a, int b){ return b-a;},Collections<int>::SortStrategy::QUICK);
        
        ArrayList<int>::ArrayListIterator* it = arr->iterator();
       
        int num = 9;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num--);
        }

        delete it;

        it = arr2->iterator();
        num = 20;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), num--);
        }

        delete it;
    }
}