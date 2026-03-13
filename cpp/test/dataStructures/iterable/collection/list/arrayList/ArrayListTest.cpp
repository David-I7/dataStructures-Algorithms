#include "dataStrucutres/iterable/collection/list/arrayList/ArrayList.h"
#include "dataStrucutres/iterable/collection/list/arrayList/ArrayListIterator.h"
#include "dataStrucutres/util/Iterator.h"
#include <gtest/gtest.h>

ArrayList<int> setupArrayList(int capacity, int to){
    ArrayList<int> arr(capacity);
    for(int i = 0 ; i < to; ++i){
        arr.add(i);
    }
    return arr;
}

namespace  {


    TEST(ArrayList, OutOfBoundIndexAccessThrowsError) {
        ArrayList<int> arr = setupArrayList(10, 8);

        EXPECT_THROW(arr.get(9), std::out_of_range);
        EXPECT_THROW(arr.get(-1), std::out_of_range);
    }

    TEST(ArrayList, ClearResizesArrayToZero) {
        ArrayList<int> arr = setupArrayList(10, 8);

        arr.clear();
        EXPECT_EQ(arr.size(),0);
    }

    TEST(ArrayList, IteratorPrintsAllElements){
         ArrayList<int> arr = setupArrayList(10, 8);

         Iterator<int>* it = arr.iterator();

         while(it->hasNext()){
            std::cout << it->next() << ", ";
         }
    }

}