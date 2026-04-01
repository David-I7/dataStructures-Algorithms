#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <ostream>
#include <utility>
#include <iostream>

// Multimea este construtita ca un array de liste inlantuite

// Exemplu:
// [Node*,Node*,nullptr,Node*,nullptr,...]
// Unde: Node* poate sa pointeze catre mai multe noduri daca au avut acelasi hashIndex
//       nullptr inseamna ca niciun element din multime nu are hashIndex-ul respectiv

template <typename T>
class Multime{
   
    public:
        ~Multime(){
            freeElements();
        }

        Multime(): capacity(10){
            allocElements(capacity);
        }

        // Conversia unui element la multimea cu un singur element
        Multime(const T& element): capacity(1){
            allocElements(capacity);
            elements[0] = new Node(element);
            size++;
        }
        
        // Conversia unui element la multimea cu un singur element folosind rvalues
        Multime(T&& element): capacity(1){
            allocElements(capacity);
            elements[0] = new Node(std::move(element));
            size++;
        }

        // Copy constructor
        Multime(const Multime<T>&other){
            allocElements(other.capacity);
            copyElements(other.elements,elements, capacity);
        }
        
        // Move constructor
        Multime(Multime<T>&&other){
            allocElements(other.capacity);
            moveElements(other.elements,other.capacity, elements);
        }

        // Copy assignment
        void operator=(const Multime<T>&other){
            freeElements();
            allocElements(other.capacity);
            copyElements(other.elements,elements, capacity);
        }
        
        // Move assignment
        void operator=(Multime<T>&&other){
            freeElements();
            allocElements(other.capacity);
            moveElements(other.elements,other.capacity, elements);
        }
    
        // Adauga un element in multime
        Multime<T>& operator<<(const T& el){
            std::size_t h = hashIndex(el,capacity);

            Node* node = elements[h];
            if (node == nullptr) {
                elements[h] = new Node(el);
                ++size;
            }else{
                // Verific ca elementul sa nu fie deja in multime 
                if (find(node, el) == nullptr){
                    getTail(node)->setNext(new Node(el));
                    ++size;
                }
            };

            checkResize();
            return *this;
        }
        
        // Sterge un element din multime
        Multime<T>& operator>>(const T& el){
            if (size == 0) return *this;

            std::size_t h = hashIndex(el,capacity);

            Node* node = elements[h];
            if (node == nullptr) {
                // Elementul nu se afla in multime
                return *this;
            }else{
                // Verific ca elementul sa fie in multime  

                if (node->getData() == el){
                    // Daca elementul este capul listei
                    elements[h] = node->getNext();
                    node->setNext(nullptr);
                    delete node;
                }else{
                    // Daca elementul nu este capul listei
                    // Gasesc nodul care pointeaza catre element pentru a restaura legaturile listei inlantuite
                    Node* before = findBefore(node, el);

                    // Elementul nu se afla in multime
                    if (before == nullptr) return *this;

                    // Restaurez legaturile listei
                    // Before nu o sa fie niciodata coada listei
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

        // Multimile sunt egale daca au aceleasi elemente
        // Trebuie sa aibe aceeasi marime (size), dar nu neaparat si capacity
        bool operator==(const Multime<T>& other){
            if (other.size != size) return false;

            return containsAll(this, other);
        }

        // Returneaza adevarat daca partea stanga a operatiei este o submultime stricta a partii drepti 
        bool operator<(const Multime<T>& other){
            if (size >= other.size) return false;

            return containsAll(this, other);
        }
        
        // Returneaza adevarat daca partea stanga a operatiei este o submultime a partii drepti 
        bool operator<=(const Multime<T>& other){
            if (size > other.size) return false;

            return containsAll(this, other);
        }
        
        // Returneaza adevarat daca partea stanga a operatiei este un superset strict a partii drepti 
        bool operator>(const Multime<T>& other){
            if (other.size <= size) return false;

            return containsAll(other, this);
        }
        
        // Returneaza adevarat daca partea stanga a operatiei este un superset a partii drepti 
        bool operator>=(const Multime<T>& other){
            if (other.size > size) return false;

            return containsAll(other, this);
        }

        // Reuniune pe multimi
        Multime<T> operator+(const Multime<T>& other){
            // Copiez multimea other
            Multime<T> reuniune = other;

            // Adaug elementele din multimea mea
            for(std::size_t i = 0 ; i < capacity;++i){
                Node* node = elements[i];
                if (node == nullptr) continue;

                do{
                    reuniune << node->getData();
                    node=node->getNext(); 
                }while(node != nullptr);
            }


            return reuniune;
        }
        
        // Intersectie pe multimi
        Multime<T> operator*(const Multime<T>& other){
            Multime<T> intersectie;

            // Adaug elementele din multimea mea
            for(std::size_t i = 0 ; i < capacity;++i){
                Node* node = elements[i];
                if (node == nullptr) continue;

                do{
                    if (other.contains(node->getData())){
                        intersectie << node->getData();
                    }
                    node= node->getNext();
                }while(node != nullptr);
            }


            return intersectie;
        }

        // Verifica apartenta in multime a elelemtului
        bool contains(const T& el){
            if (size == 0) return false;

            std::size_t h = hashIndex(el, capacity);

            Node* node = elements[h];
           
            return find(node, el) != nullptr;
        }

        // Afiseaza lista pe standard output
        friend std::ostream& operator<<(std::ostream& os,const Multime<T>& multime){
            if (multime.size == 0) os << "{}";
            else {
                os << '{';
                
                // Numar elementele pentru a nu afisa o virgula in plus la final (ex: {1,2,3,})
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
        // Clasa interna folosita pentru a implementa o lista inlantuita
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

        // Returneaza true daca toate elementele din a se gasesc in b
        bool containsAll(const Multime<T>& a, const Multime<T>& b){
            for(std::size_t i = 0; i < a.capacity;++i){
                Node* node = a.elements[i];
                if (node == nullptr) continue;

                while(node != nullptr){
                    if (!b.contains(node->getData())) return false;
                    node=node->getNext();
                }
            }

            return true;
        }

        // Elibereaza memoria
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

        // Alloca memorie. Metoda este apelata doar daca elements == nulptr  
        void allocElements(std::size_t capacity){
            // Initializeaza memoria cu nullptr
            elements = new Node*[capacity]();
            this->capacity = capacity;
        }

        // Realloca memorie pentru elementele multimii
        void reallocElements(std::size_t newCapacity){
            // Initializeaza memoria cu nullptr
            Node** buff = new Node*[newCapacity]();
        
            // Mut elementele din memoria veche de lungime capacity
            // in memoria noua de lungime newCapacity
            moveElements(elements, capacity, buff, newCapacity);
            
            delete[] elements; 
            elements = buff;
            capacity = newCapacity;
        }

        // Facem un deep copy (nu copiem doar pointerii)
        void copyElements(Node** fromBuf,Node** toBuf,std::size_t capacity){
            for(std::size_t i = 0; i < capacity; ++i){
                
                Node* node = fromBuf[i];
                if (node == nullptr) continue;

                // Hashul este acelasi pentru ca lungimea este aceeasi
                std::size_t h = i;
                
                // Copiez nodul recursiv
                Node* copy = deepCopy(node);
                toBuf[h] = copy;
            }
        }

        Node* deepCopy(const Node* other){
            if (other == nullptr) return nullptr;

            Node* copy = deepCopy(other->next);

            return new Node(other->data,copy);
        }

        void moveElements(Node** oldBuf,std::size_t oldCapacity,Node** newBuf,std::size_t newCapacity){
            // Mut elementele din memoria veche de lungime capacity
            // in memoria noua de lungime newCapacity
            for(std::size_t i = 0; i < capacity; ++i){
                
                Node* node = oldBuf[i];
                if (node == nullptr) continue;

                do{
                    // Calculez noul hash
                    std::size_t h = hashIndex(node->getData(), newCapacity);
                    
                    // Inserez nodul in memoria noua alocata 
                    Node* tail = getTail(newBuf[h]);
                    if (tail == nullptr) {newBuf[h] = node; tail = node;}
                    else tail->setNext(node);
            
                    // Trec la urmatorul element in lista inlantuita
                    node = node->getNext();
                    // Sterg legatura catre nodul urmator daca exista
                    if (tail->getNext()){
                        tail->getNext()->setNext(nullptr);
                    }
                
                }while (node != nullptr);

                oldBuf[i] = nullptr;
            }
        }

        // Gasesc coada din lista
        Node* getTail(Node* node){
            while(node->getNext()){
                node = node->getNext();
            }
            return node;
        }

        // Gasesc nodul ce contine elementul
        Node* find(Node* root,const T& el){
            while (root != nullptr) {
                if (root->getData() == el) return root;
                root = root->getNext();
            }

            return root;
        }

        // Gasesc nodul care pointeaza catre nodul ce contine elementul 
        Node* findBefore(Node* root,const T& el){
            if (root == nullptr) return nullptr;

            while (root->getNext() != nullptr) {
                if (root->getNext()->getData()== el) return root;
                root = root->getNext();
            }

            return nullptr;
        }

        // Verfica daca este necesar sa alloc sau sa reduc din spatiu
        // Mai mult saptiu -> mai putine coliziuni
        // Mai putin spatiu -> mai multe coliziuni
        void checkResize(){
            if (size >= 3 * capacity){
                reallocElements(2 * capacity);
            }else if (capacity > 10 && size <= capacity / 3){
                reallocElements(std::max(capacity / 2,(std::size_t)10));
            }
        }
};


int main(){
    Multime<int> multime;

    multime << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 11 << 12 << 13 << 14 << 15 << 16 ;
    std::cout << multime;

    multime >> 1 >> 5 >> 10;
    std::cout << multime;

}