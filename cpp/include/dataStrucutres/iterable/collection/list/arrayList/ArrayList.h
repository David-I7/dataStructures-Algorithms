#pragma once

#include "../List.h"
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <new>
#include <malloc.h>
#include "../../../../Object.h"

template <typename E>
class ArrayList : public List<E>, public Object
{
    int m_capacity = 2;
    int m_size = 0;
    E* m_items;

    void resize(int newCapacity){
        E* newData = new E[newCapacity];
        
        int to = m_size < newCapacity ? m_size : newCapacity;
        for(int i = 0 ; i < to;++i){
            newData[i] = std::move(m_items[i]);
        }
    
        for(int i = 0; i < m_capacity;++i){
            m_items[i].~E();
        }
        delete [] m_items;

        m_capacity = newCapacity;
        m_items = newData;
    }

    public:
    ArrayList(){
        m_items = new E[m_capacity];
    }
    ArrayList(int capacity){
        m_capacity = capacity;
        m_items = new E[m_capacity];
    }
    ~ArrayList(){
        for(int i = 0; i < m_capacity;++i){
            m_items[i].~E();
        }

    }

    //Iterator<E> iterator() = 0;
    void forEach(const std::function<void(const E&)>& action) const override {
        for(int i = 0; i < m_size;++i){
            action(m_items[i]);
        }
    };
    bool add(const E& e) override{
        set(m_size, e);
        return true;
    };
    // bool addAll(const Collection<E>& e){
    //     for(int i = 0; i < e.size();++i){
    //         add()
    //     }
    // };
    void clear() override{
        if (m_capacity == 0) return;
        resize(0);
        m_size = 0;
    };
    bool isEmpty() const override{
        return m_size == 0;
    };
    int size() const override{
        return m_size;
    };
    //void add(int index, E element) = 0;
    //List<E> subList(int fromIndex, int toIndex) = 0;
    void set(int index, const E& element) override{
        if (index < 0 || index > m_size) {
            std::stringstream ss;
            ss << "Index " << index << " out of bounds for size " << m_size;
            throw std::out_of_range(ss.str());
        };

        if (index != m_size){
            E& old = m_items[index];
            old.~E();
        }else{
            if (m_capacity == 0){
                resize(2);
            }
            ++m_size;
        }

        // construct new element in-place
        new (&m_items[index]) E(element);

        if (m_size == m_capacity){
            resize( m_capacity * 2);
        }
    };

    friend std::ostream& operator<<( std::ostream& os,const ArrayList<E>& obj){
        if (obj.m_size == 0){
            os << "[]\n";
            return os;
        }

        os << "[";
        int i = 0;
        for(; i < obj.m_size -1;++i){
            os << obj.m_items[i] << ", ";
        }
        os << obj.m_items[i] << "]\n";

        return os;
    };
};