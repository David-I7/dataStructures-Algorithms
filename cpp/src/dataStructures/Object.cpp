#include "Object.h"

#include <cstddef>
#include <functional>

bool Object::equals(const Object& obj) const{
    return &(*this) == &obj;
}

bool Object::operator==(const Object& obj) const{
    return this->equals(obj);
}

std::size_t Object::hashCode() const{
    return std::hash<const Object*>{}(this);
}

 std::ostream& operator<<( std::ostream& os,const Object& obj){
    os << "Object{}\n";
    return os;
}