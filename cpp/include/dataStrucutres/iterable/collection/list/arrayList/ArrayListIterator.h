#pragma once

#include "dataStrucutres/util/ListIterator.h"
#include <stdexcept>

template<typename E>
class ArrayList;

template <typename E> 
class ArrayListIterator: public ListIterator<E>{
    ArrayList<E>* m_source;
    int m_index = 0;

    public:
    ArrayListIterator(ArrayList<E>* list): m_source(list){};

    bool hasPrevious() override{
        return m_index - 1 >= 0 && !m_source->isEmpty();
    };
    E& previous() override{
        if (hasPrevious())
            return m_source->get(--m_index);
        else throw std::out_of_range("No more element left to traverse");
    };
    bool nextIndex() override{
        return m_index;
    };
    bool previousIndex() override{
        return m_index - 1;
    };
    E& next() override{
        if (hasNext())
            return m_source->get(m_index++);
        else throw std::out_of_range("No more element left to traverse");
    };
    bool hasNext() override{
        return m_index < m_source->size();
    };
    void forEachRemaining(const std::function<void(E&)>& action) override{
        while(hasNext()){
            action(next());
        }
    };
};