#pragma once

#include "../Collection.h"

template <typename E>
class List : public Collection<E>
{
public:
    using Collection<E>::add; // restore Collection overloads

    virtual bool add(int index, const E& element) = 0;
    virtual bool add(int index, E&& element) = 0;
    virtual List<E>* subList(int fromIndex, int toIndex) = 0;
    virtual void set(int index, const E& element) = 0;
    virtual void set(int index,  E&& element) = 0;
    virtual E& get(int index) = 0;
    virtual int indexOf(const E& e) = 0;
};