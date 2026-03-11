#pragma once

#include <functional>

template <typename E>
class Iterator
{

public:
    virtual const E& next() = 0;
    virtual bool hasNext() = 0;
    virtual void remove() = 0;
    virtual void forEachRemaining(const std::function<void(E&)>& action) = 0;
};