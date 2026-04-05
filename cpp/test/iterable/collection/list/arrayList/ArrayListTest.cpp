#include "java/iterable/collection/list/arrayList/ArrayList.h"
#include <gtest/gtest.h>
#include <functional>



namespace  {
    template<typename T> 
    ArrayList<T> setupArrayList(int capacity, std::function<T(int)> mapper){
        ArrayList<T> arr(capacity);
        for(int i = 0 ; i < capacity; ++i){
            arr.add(mapper(i));
        }
        return arr;
    }

    int intMapper(int index){return index;}
    int fixedMapper(int index){return 1;}


    TEST(ArrayList, OutOfBoundIndexAccessThrowsError) {
        ArrayList<int> arr = setupArrayList<int>(10, intMapper);

        EXPECT_THROW(arr.get(10), std::out_of_range);
        EXPECT_THROW(arr.get(-1), std::out_of_range);
    }

    TEST(ArrayList, ClearResizesArrayToZero) {
        ArrayList<int> arr = setupArrayList<int>(10, intMapper);

        EXPECT_EQ(arr.size(), 10);
        arr.clear();
        EXPECT_EQ(arr.size(),0);
    }

    TEST(ArrayList, ShouldReturnTheFirstOccurrenceFound){
        ArrayList<int> arr = setupArrayList<int>(10, fixedMapper);

        int index = arr.indexOf(1);
        EXPECT_EQ(index, 0);
    }

    TEST(ArrayList, ShouldReturnMinusOneIfItemIsNotFound){
        ArrayList<int> arr = setupArrayList<int>(10, fixedMapper);

        int index = arr.indexOf(2);
        EXPECT_EQ(index, -1);
    }

    TEST(ArrayList, ShouldReturnTrueIfTheItemIsPresentInTheContainer){
        ArrayList<int> arr = setupArrayList<int>(10, fixedMapper);

        EXPECT_TRUE(arr.contains(1));
    }

    TEST(ArrayList, ShouldReturnFalseIfTheItemIsNotPresentInTheContainer){
        ArrayList<int> arr = setupArrayList<int>(10, fixedMapper);

        EXPECT_FALSE(arr.contains(2));
    }

    TEST(ArrayList, ShouldRemoveAndDecreaseSizeOfContainer){
        ArrayList<int> arr = setupArrayList<int>(10, fixedMapper);

        int prevSize = arr.size();
        bool removed = arr.remove(1);
        EXPECT_EQ(arr.size(),prevSize -1);
        EXPECT_TRUE(removed);
    }

    TEST(ArrayList, ShouldNotRemoveNorDecreaseSizeOfContainer){
        ArrayList<int> arr = setupArrayList<int>(10, fixedMapper);

        int prevSize = arr.size();
        int removed = arr.remove(2);
        EXPECT_EQ(arr.size(),prevSize );
        EXPECT_FALSE(removed);
    }

    TEST(ArrayList, ShouldAddElementAtTheEndOfTheContainer){
        ArrayList<int> arr = setupArrayList<int>(10, fixedMapper);

        int prevSize = arr.size();
        arr.add(10);
        EXPECT_EQ(arr.size(),prevSize + 1 );
        EXPECT_EQ(arr.get(arr.size() -1), 10);
    }

    TEST(ArrayList, ShouldAddElementAtSpecifiedIndex){
        ArrayList<int> arr = setupArrayList<int>(10, fixedMapper);

        int prevSize = arr.size();
        arr.add(2,10);
        EXPECT_EQ(arr.size(),prevSize + 1 );
        EXPECT_EQ(arr.get(2), 10);
    }

    TEST(ArrayList, ShouldReturnShallowCopySublist){
        ArrayList<int*> arr = setupArrayList<int*>(10, [](int idx)->int* {return new int(idx);});

        List<int*>* arr2 = arr.subList(2, 5);
        EXPECT_EQ(arr2->size(),3);
        EXPECT_EQ(arr2->get(0)[0],2);
        EXPECT_EQ(arr2->get(1)[0],3);
        EXPECT_EQ(arr2->get(2)[0],4);

        arr2->get(0)[0] = 20;
        EXPECT_EQ(arr.get(2)[0], 20);
        
        delete arr2;
        arr.forEach([](const int* i)->void{free((void*)i);});
    }

}