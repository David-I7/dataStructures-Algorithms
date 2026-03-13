#pragma once

#include <functional>
#include "../Object.h"

template <typename E>
class Iterator: public Object
{

public:
    virtual E& next() = 0;
    virtual bool hasNext() = 0;
    virtual void forEachRemaining(const std::function<void(E&)>& action) = 0;
};