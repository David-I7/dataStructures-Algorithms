#include "java/iterable/collection/list/arrayList/ArrayList.h"
#include <gtest/gtest.h>

namespace{
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


    TEST(ArrayListIterator,IteratesThroughAllElements){
        ArrayList<int>arr = setupArrayList<int>(10, intMapper);
        ArrayList<int>::ArrayListIterator* it = arr.iterator();
        
        int i = 0;
        while(it->hasNext()){
            EXPECT_EQ(it->next(), i++);
        }

        delete it;
    }
}