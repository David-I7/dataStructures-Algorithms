#include <cstdlib>
#include <functional>
#include <ostream>
#include <utility>
#include <iostream>

template <typename T>
class Multime{
   
    public:
        ~Multime(){
            freeElements();
        }

        Multime(): capacity(10){
            reallocElements(capacity);
        }

        Multime(const T& element): capacity(1){
            reallocElements(capacity);
            elements[0] = new Node(element);
            size++;
        }
        
        Multime(T&& element): capacity(1){
            reallocElements(capacity);
            elements[0] = new Node(std::move(element));
            size++;
        }
    
        Multime<T>& operator<<(const T& el){
            std::size_t h = hashIndex(el,capacity);

            Node* node = elements[h];
            if (node == nullptr) {
                elements[h] = new Node(el);
                ++size;
            }else{
                // Verificam ca elementul sa nu fie deja in multime,  
                if (find(node, el) == nullptr){
                    getTail(node)->setNext(new Node(el));
                    ++size;
                }
            };

            checkResize();
            return *this;
        }
        
        Multime<T>& operator>>(const T& el){
            if (size == 0) return *this;

            std::size_t h = hashIndex(el,capacity);

            Node* node = elements[h];
            if (node == nullptr) {
                return *this;
            }else{
                // Verificam ca elementul sa fie in multime,  

                if (node->getData() == el){
                    elements[h] = node->getNext();
                    node->setNext(nullptr);
                    delete node;
                }else{
                    Node* before = findBefore(node, el);
                    if (before == nullptr) return *this;

                    Node* deleted = before->getNext();
                    before->setNext(deleted->getNext());
                    deleted->setNext(nullptr);
                    delete deleted;
                }   
            }
            
            --size;
            checkResize();
            return *this;
        }

        bool contains(const T& el){
            if (size == 0) return false;

            std::size_t h = hashIndex(el, capacity);

            Node* node = elements[h];
            // Elementul este deja in multime
            return find(node, el) != nullptr;
        }

        friend std::ostream& operator<<(std::ostream& os,const Multime<T>& multime){
            if (multime.size == 0) os << "{}";
            else {
                os << '{';
                
                std::size_t count = 0;
                for(std::size_t i=0; i < multime.capacity;++i){
                    Node* node = multime.elements[i];
                    if (node == nullptr) continue;
                    
                    while (node != nullptr) {
                        if (count == multime.size -1){
                            os << node->getData();
                            ++count;
                            break;
                        }
                        os << node->getData() << ", ";
                        ++count;
                        node= node->getNext();
                    }

                    if (count == multime.size) break;
                }

                os << '}';
            }

            return os;
        }


    private:
        class Node{
            public:
           
            Node(T data, Node* next = nullptr):data(data),next(next){}
            
            Node(T&& data, Node* next = nullptr):data(std::move(data)),next(next){}
            
            ~Node(){}
            
            Node* getNext(){
                return next;
            }

            void setNext(Node* next){
                this->next = next;
            }

            const T& getData(){
                return data;
            }

            private:
                T data;
                Node* next;
        };

        std::size_t capacity=0, size=0;
        Node** elements = nullptr;
        std::hash<T> hash;
       
        inline std::size_t hashIndex(const T& el, std::size_t capacity){
            return hash(el) % capacity;
        }

        void freeElements(){
            for(std::size_t i = 0 ; i < capacity; ++i){
                Node* node = elements[i];
                if (node == nullptr) continue;

                do{
                    Node* next = node->getNext();
                    node->setNext(nullptr);
                    delete node;
                    node = next;
                }while (node != nullptr);
                
                elements[i] = nullptr;
            }

            delete [] elements;
            size = 0;
            capacity =0;
            elements = nullptr;
        }

        void reallocElements(std::size_t newCapacity){
            // initializeaza memoria cu nullptr
            Node** buff = new Node*[newCapacity]();
            
            if (elements == nullptr){
                elements = buff;
                capacity = newCapacity;
                return;
            }

            for(std::size_t i = 0; i < capacity; ++i){
                
                Node* node = elements[i];
                if (node == nullptr) continue;

                do{
                    std::size_t h = hashIndex(node->getData(), newCapacity);
                    
                    Node* tail = getTail(buff[h]);
                    if (tail == nullptr) {buff[h] = node; tail = node;}
                    else tail->setNext(node);
            
                    // trecem la urmatorul element in lista
                    node = node->getNext();
                    // stergem legatura
                    if (tail->getNext()){
                        tail->getNext()->setNext(nullptr);
                    }
                
                }while (node != nullptr);

                elements[i] = nullptr;
            }

            delete[] elements; 
            elements = buff;
            capacity = newCapacity;
        }

        Node* getTail(Node* node){
            while(node->getNext()){
                node = node->getNext();
            }
            return node;
        }

        Node* find(Node* root,const T& el){
            while (root != nullptr) {
                if (root->getData() == el) return root;
                root = root->getNext();
            }

            return root;
        }

        Node* findBefore(Node* root,const T& el){
            if (root == nullptr) return nullptr;

            while (root->getNext() != nullptr) {
                if (root->getNext()->getData()== el) return root;
                root = root->getNext();
            }

            return nullptr;
        }

        void checkResize(){
            if (size >= 3 * capacity){
                reallocElements(2 * capacity);
            }else if (capacity > 10 && size <= capacity / 3){
                reallocElements(capacity / 2);
            }
        }
};


int main(){
    Multime<int> multime;

    multime << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10<< 11 << 11 << 12 << 13 << 14 << 15 << 16 ;
    std::cout << multime;

    multime >> 1 >> 5 >> 10;
    std::cout << multime;

}