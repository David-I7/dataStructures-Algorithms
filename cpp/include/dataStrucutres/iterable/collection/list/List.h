#pragma once

#include "../Collection.h"

template <typename E>
class List : public Collection<E>
{
public:
    //virtual void add(int index, const E& element) = 0;
    //virtual List<E> subList(int fromIndex, int toIndex) = 0;
    virtual const E& set(int index, const E& element) = 0;
};