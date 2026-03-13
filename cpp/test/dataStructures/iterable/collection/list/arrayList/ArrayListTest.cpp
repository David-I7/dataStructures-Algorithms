#include "dataStrucutres/iterable/collection/list/arrayList/ArrayList.h"
#include <gtest/gtest.h>
#include <exception>

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

}