#pragma once

#include "../util/Iterator.h"

#include <functional>
#include "../Object.h"

template <typename E>
class Iterable: public Object
{

public:
    virtual Iterator<E>* iterator() = 0;
    virtual void forEach(const std::function<void(const E&)>& action) const = 0;
    virtual ~Iterable() = default;
};