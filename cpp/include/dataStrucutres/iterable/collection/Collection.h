#pragma once

#include "../Iterable.h"

template <typename E>
class Collection : public Iterable<E>
{

public:
    virtual bool add(const E& e) = 0;
    //virtual bool addAll(const Collection<E>& e) = 0;
    virtual void clear() = 0;
    virtual bool isEmpty() const = 0;
    virtual int size() const = 0;
    //virtual bool contains() = 0;
};