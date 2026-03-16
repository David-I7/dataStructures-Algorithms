#pragma once

#include "./Iterator.h"

template <typename E>
class ListIterator: public Iterator<E>{
    virtual bool hasPrevious() = 0;
    virtual E& previous() = 0;
    virtual bool nextIndex() = 0;
    virtual bool previousIndex() = 0;
};