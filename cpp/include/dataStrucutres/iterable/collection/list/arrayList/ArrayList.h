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

    void checkIndex(int index,int end = -1){
        end = end == -1 ? m_size : end;
        if (index < 0 || index > end) {
            std::stringstream ss;
            ss << "Index " << index << " out of bounds for size " << m_size;
            throw std::out_of_range(ss.str());
        };
    }


    void checkSize(){
        if (m_capacity == 0){
                resize(2);
        }else if (m_size == m_capacity){
            resize( m_capacity * 2);
        }
    }

    void shuffle(int start){
        if (start == m_size) return;
        
        new (&m_items[m_size]) E(std::move(m_items[m_size-1]));

        for(int i = m_size-1; i > start; --i){
            m_items[i] = std::move(m_items[i-1]);
        }
    }

    void shuffleBack(int start){
        for(int i = start; i < m_size-1; --i){
            m_items[i] = std::move(m_items[i+1]);
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
    bool add(int index,const E& e) override{
        checkIndex(index);
        
        checkSize();

        shuffle(index);

        new (&m_items[index]) E(e);

        return true;
    };
    bool add(int index,E&& e) override{
        checkIndex(index);
        
        checkSize();

        shuffle(index);

        new (&m_items[index]) E(std::move(e));

        return true;
    };
    bool add(const E& e) override{
        set(m_size, e);
        return true;
    };
    bool add(E&& e) override{
        set(m_size, std::move(e));
        return true;
    };
    // bool addAll(const Collection<E>& e) override{
    //     for(int i = 0; i < e.size();++i){
    //         add(e.)
    //     }
    // };
    // bool removeAll(const Collection<E>& e) override{

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
    List<E>* subList(int fromIndex, int toIndex) override{
        int capacity = toIndex - fromIndex;
        if (capacity < 0) throw std::invalid_argument("toIndex argument must be greater than fromIndex");
        checkIndex(fromIndex);
        checkIndex(toIndex);

        List<E>* subList = new ArrayList<E>(capacity);
        for(int i = fromIndex; i < toIndex; ++i){
            subList->add(m_items[i]);
        };

        return subList;
    };
    void set(int index, const E& element) override{
        checkIndex(index);
        
        if(index == m_size){
            checkSize();
            ++m_size;
        }

        // copy element in-place
        new (&m_items[index]) E(element);
    };
    void set(int index, E&& element) override{
        checkIndex(index);
        
        if(index == m_size){
            checkSize();
            ++m_size;
        }

        // move element in-place
        new (&m_items[index]) E(std::move(element));
    };
    
    E& get(int index) override{
        checkIndex(index,m_size - 1);
        return m_items[index];
    }

    int indexOf(const E& e) override{
        for(int i = 0; i < m_size;++i){
            if (e == m_items[i]) return i;
        }  
        return -1;
    }

    virtual bool contains(const E& e) override{
        return indexOf(e) != -1;
    };

    virtual bool remove(const E& e) override{
        int index = indexOf(e);
        if (index == -1) return false;

        m_items[index].~E();
        shuffleBack(index);
        --m_size;
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