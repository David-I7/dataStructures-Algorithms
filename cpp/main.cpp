#include "include/dataStrucutres/iterable/collection/list/arrayList/ArrayList.h"

#include <iostream>

int main()
{
    ArrayList<int> arr;

    arr.add(10);
    arr.add(20);

    arr.forEach([](int i) {std::cout << i; });

    return 0;
}