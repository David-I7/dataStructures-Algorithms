#pragma once

#include "./Iterator.h"

template <typename E>
class ListIterator: public Iterator<E>{
    virtual bool hasPrevious() = 0;
    virtual E& previous() = 0;
    virtual int nextIndex() = 0;
    virtual int previousIndex() = 0;
    virtual void set(const E& e) = 0;
    virtual void add(const E& e) = 0;
};