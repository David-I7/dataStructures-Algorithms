#pragma once

#include "../List.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <new>
#include <cstdlib>
#include "../../../../Object.h"

template <typename E>
class ArrayList : public List<E>
{
    int m_capacity = 2;
    int m_size = 0;
    E* m_items;

    void freeData(){
        for(int i = 0; i < m_size;++i){
            m_items[i].~E();
        }
        free(m_items);
    }

    void resize(int newCapacity){
        E* newData = allocMemory(newCapacity);
      
        if(newData == nullptr && newCapacity != 0) return;
        
        int to = m_size < newCapacity ? m_size : newCapacity;
        for(int i = 0 ; i < to;++i){
            new (&newData[i]) E(std::move(m_items[i]));
        }
    
        freeData();

        m_capacity = newCapacity;
        m_items = newData;
    }

    E* allocMemory(std::size_t capacity){
        return (E*) malloc(sizeof(ArrayList<E>) * capacity);
    }

    void _set(int index, const E& element){
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
            }else if (++m_size == m_capacity){
                resize( m_capacity * 2);
            }
        }
    }

    public:
    ArrayList(){
        m_items = allocMemory( m_capacity);
    }
    ArrayList(int capacity){
        m_capacity = capacity;
        m_items = allocMemory( m_capacity);
    }
    ~ArrayList(){
        freeData();
    }
    ArrayList(const ArrayList<E>& other){
        m_capacity = other.m_capacity;
        m_items = allocMemory( m_capacity);
        m_size = other.m_size;

        for(int i = 0; i < m_size;++i){
            new (&m_items[i]) E(other.m_items[i]);
        }
    }
    ArrayList(ArrayList<E>&& other){
        m_capacity = other.m_capacity;
        m_items = other.m_items;
        m_size = other.m_size;
        other.m_items = nullptr;
        other.m_size = 0;
        other.m_capacity =0;
    }

    void operator=(const ArrayList<E>& other){
        if(m_capacity > 0){
            freeData();
        }
        m_capacity = other.m_capacity;
        m_items = allocMemory( m_capacity);
        m_size = other.m_size;

        for(int i = 0; i < m_size;++i){
            new (&m_items[i]) E(other.m_items[i]);
        }
    }

    void operator=(ArrayList<E>&& other){
        if(m_capacity > 0){
            freeData();
        }
        m_capacity = other.m_capacity;
        m_items = other.m_items;
        m_size = other.m_size;
        other.m_items = nullptr;
        other.m_size = 0;
        other.m_capacity =0;
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
    bool add(E&& e) override{
        set(m_size, std::move(e));
        return true;
    };
    // bool addAll(const Collection<E>& e){
    //     for(int i = 0; i < e.size();++i){
    //         add()
    //     }
    // };
    void clear() override{
        if (m_capacity == 0) return;
        freeData();
        m_capacity = 0;
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
        _set(index, element);

        // copy element in-place
        new (&m_items[index]) E(element);
    };
    void set(int index, E&& element) override{
        _set(index,element);

        // move element in-place
        new (&m_items[index]) E(std::move(element));
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
        os << obj.m_items[i] << "]";

        return os;
    };
};