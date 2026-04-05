#pragma once

#include <cstddef>
#include <ostream>

class Object{
    public:
        Object() = default;
        ~Object() = default;

        bool equals(const Object& obj) const;
        std::size_t hashCode() const;
        friend  std::ostream& operator<<( std::ostream& os,const Object& obj);
        bool operator==(const Object& obj) const;
};