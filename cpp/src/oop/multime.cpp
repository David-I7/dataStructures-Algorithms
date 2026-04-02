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
// Unde: Node* poate sa pointeze catre mai multe noduri daca au avut acelasi hashIndex.
//       nullptr inseamna ca niciun element din multime nu are hashIndex-ul respectiv.

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
        
        // Conversia unui element la multimea cu un singur element folosind move
        Multime(T&& element): capacity(1){
            allocElements(capacity);
            elements[0] = new Node(std::move(element));
            size++;
        }

        // Constructor de copiere
        Multime(const Multime<T>&other){
            allocElements(other.capacity);
            if (other.size > 0){
                copyElements(other.elements,elements, capacity);
            }
            size = other.size;
        }
        
        // Constructor move
        Multime(Multime<T>&&other){
            elements = other.elements;
            capacity = other.capacity;
            size = other.size;

            other.elements = nullptr;
            other.capacity = 0;
            other.size = 0;
        }

        // Atribuire prin copiere
        Multime& operator=(const Multime<T>&other){
            if(&other == this) return *this;

            freeElements();
            allocElements(other.capacity);
            if (other.size > 0){
                copyElements(other.elements,elements, capacity);
            }
            size = other.size;

            return *this;
        }
        
        // Atribuire prin move
        Multime& operator=(Multime<T>&&other){
            if(&other == this) return *this;

            freeElements();
            elements = other.elements;
            capacity = other.capacity;
            size = other.size;

            other.elements = nullptr;
            other.capacity = 0;
            other.size = 0;

            return *this;
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
                // Codul poate fi optimizat, insa am preferat lizibilitatea  
                if (node->find(el) == nullptr){
                    // Adauga elementul nou la coada listei inaintuite
                    node->getTail()->setNext(new Node(el));
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
                    // Daca elementul este capul listei inlantuite
                    elements[h] = node->getNext();
                    node->setNext(nullptr);
                    delete node;
                }else{
                    // Daca elementul nu este capul listei inlantuite
                    // Gasesc nodul care pointeaza catre element pentru a restaura legaturile listei inlantuite
                    Node* before = node->findBefore(el);

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
 
            return containsAll(*this, other);
        }

        // Returneaza adevarat daca partea stanga a operatiei este o submultime stricta a partii drepti 
        bool operator<(const Multime<T>& other){
            if (size >= other.size) return false;

            return containsAll(*this, other);
        }
        
        // Returneaza adevarat daca partea stanga a operatiei este o submultime a partii drepti 
        bool operator<=(const Multime<T>& other){
            if (size > other.size) return false;

            return containsAll(*this, other);
        }
        
        // Returneaza adevarat daca partea stanga a operatiei este un superset strict a partii drepti 
        bool operator>(const Multime<T>& other){
            if (other.size >= size) return false;

            return containsAll(other, *this);
        }
        
        // Returneaza adevarat daca partea stanga a operatiei este un superset a partii drepti 
        bool operator>=(const Multime<T>& other){
            if (other.size > size) return false;
            
            return containsAll(other, *this);
        }

        // Reuniune pe multimi
        Multime<T> operator+(const Multime<T>& other){
            // Cazurile triviale
            if (other.size == 0 && size == 0) return Multime<T>();
            else if (other.size == 0) return Multime<T>(*this);
            else if (size == 0) return Multime<T>(other);

            // Copiez multimea other
            Multime<T> reuniune = other;

            // Adaug elementele din multimea mea (this)
            for(std::size_t i = 0 ; i < capacity;++i){
                Node* node = elements[i];
                if (node == nullptr) continue;

                do{
                    // Adaug elementul in reuniune. Operatia de adaugare verfica pentru elemente duplicate deja
                    reuniune << node->getData();
                    node=node->getNext(); 
                }while(node != nullptr);
            }

            return reuniune;
        }
        
        // Intersectie pe multimi
        Multime<T> operator*(const Multime<T>& other){
            Multime<T> intersectie;

            const Multime<T>& minSet = (size <= other.size) ? *this : other;
            
            // Adaug elementele din multimea cu mai putine elemente care se gasesc in cealalta multime
            for(std::size_t i = 0 ; i < minSet.capacity;++i){
                Node* node = minSet.elements[i];
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

        // Diferenta pe multimi. Partea stanga mai putin partea dreapta
         Multime<T> operator-(const Multime<T>& other){
            // Cazurile triviale
            if (other.size == 0) return Multime<T>(*this);
            else if (size == 0) return Multime<T>();

            Multime<T> diferenta;

            // Adaug elementele din multimea mea (this) care nu se gasesc in multimea other
            for(std::size_t i = 0 ; i < capacity;++i){
                Node* node = elements[i];
                if (node == nullptr) continue;

                do{
                    if (!other.contains(node->getData())){
                        diferenta << node->getData();
                    }
                     node=node->getNext();
                }while(node != nullptr);
            }

            return diferenta;
        }

        // Verifica apartenenta in multime a elementului
        bool contains(const T& el) const{
            if (size == 0) return false;

            std::size_t h = hashIndex(el, capacity);

            Node* node = elements[h];
           
            return node && node->find(el) != nullptr;
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
                        // Daca count == multime.size - 1 => am ajus la ultimul element 
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
           
            Node(const T& data, Node* next = nullptr):data(data),next(next){}
            
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

            // Copiez elementul deep iterativ
            // Puteam face acest copy si prin copy constructor sau copy assignment, dar am preferat sa folosesc o metoda pentru lizibilitate
            Node* deepCopy(){
                Node* curCopy = new Node(data);
                Node* rootCopy = curCopy;
                Node* cur = this->getNext();

                while (cur != nullptr) {
                    curCopy->setNext(new Node(cur->data));
                    curCopy = curCopy->getNext();
                    cur=cur->getNext();
                }

                return rootCopy;
            }

            // Copiez elementul deep recursiv
            // Node* deepCopy(){
            //     return _deepCopy(this);
            // }

            // Gasesc nodul ce contine elementul el
            Node* find(const T& el){
                Node* root = this;
                while (root != nullptr) {
                    if (root->getData() == el) return root;
                    root = root->getNext();
                }

                return root;
            }

            // Gasesc nodul care pointeaza catre nodul ce contine elementul 
            Node* findBefore(const T& el){
                Node* root = this;
                while (root->getNext() != nullptr) {
                    if (root->getNext()->getData()== el) return root;
                    root = root->getNext();
                }

                return nullptr;
            }

            // Gasesc coada din lista
            Node* getTail(){
                Node* node = this;
                while(node->getNext()){
                    node = node->getNext();
                }
                return node;
            }

            private:
                T data;
                Node* next;

                // Node* _deepCopy(const Node* other){
                //     if (other == nullptr) return nullptr;

                //     Node* copy = _deepCopy(other->next);

                //     return new Node(other->data,copy);
                // }
        };

        std::size_t capacity=0, size=0;
        Node** elements = nullptr;
        std::hash<T> hash;
       
        inline std::size_t hashIndex(const T& el, std::size_t capacity) const{
            return hash(el) % capacity;
        }

        // Returneaza true daca toate elementele din a se gasesc in b
        bool containsAll(const Multime<T>& a, const Multime<T>& b){
            // Cazul trivial
            if (a.size == 0) return true;

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
        
            moveElements(elements, capacity, buff, newCapacity);
            
            delete[] elements; 
            elements = buff;
            capacity = newCapacity;
        }

        // Face un deep copy (nu copiaza doar pointerii) din fromBuf catre toBuf
        void copyElements(Node** fromBuf,Node** toBuf,std::size_t capacity){
            for(std::size_t i = 0; i < capacity; ++i){
                
                Node* node = fromBuf[i];
                if (node == nullptr) continue;

                // Hashul este acelasi pentru ca lungimea (capacity) este aceeasi
                std::size_t h = i;
                
                // Copiez nodul deep
                Node* copy = node->deepCopy();
                toBuf[h] = copy;
            }
        }

        // Muta elementele din memoria veche (oldbuf) de lungime oldCapacity in memoria noua (newBuf) de lungime newCapacity
        void moveElements(Node** oldBuf,std::size_t oldCapacity,Node** newBuf,std::size_t newCapacity){
            for(std::size_t i = 0; i < oldCapacity; ++i){
                
                Node* node = oldBuf[i];
                if (node == nullptr) continue;

                do{
                    // Calculez noul hash
                    std::size_t h = hashIndex(node->getData(), newCapacity);
                    
                    // Inserez nodul in memoria noua alocata 
                    Node* dest = newBuf[h];
                    Node* tail;
                    if (dest == nullptr) {newBuf[h] = node; tail = node;}
                    else {tail = dest->getTail(); tail->setNext(node);};
            
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

        // Verfica daca este necesar sa alloc sau sa reduc din capacity.
        // Mai mult saptiu => mai putine coliziuni.
        // Mai putin spatiu => mai multe coliziuni.
        void checkResize(){
            if (size >= 3 * capacity){
                reallocElements(2 * capacity);
            }else if (capacity > 10 && size <= capacity / 3){
                reallocElements(std::max(capacity / 2,(std::size_t)10));
            }
        }
};


int main(){
    std::cout << "=== Initializez multimile A si B ===\n"; 
    
    Multime<int> A; A << 1 << 2 << 3 << 4; 
    Multime<int> B; B << 3 << 4 << 5 << 6; 
    
    std::cout << "A = " << A << "\n"; 
    std::cout << "B = " << B << "\n"; 
    
    std::cout << "\n=== Constructor cu un singur element ===\n"; 
    Multime<int> C(10); 
    std::cout << "C = " << C << "\n"; 
    
    {
        std::cout << "\n=== Constructor de copiere ===\n"; 
        Multime<int> D = A; 
        std::cout << "D (copie a lui A) = " << D << "\n"; 
    }
    {
        Multime<int> D = A;     
        std::cout << "\n=== Constructor move  ===\n"; 
        Multime<int> E = std::move(D); 
        std::cout << "E (mutat din D) = " << E << "\n"; 
        std::cout << "D (dupa move) = " << D << "\n"; 
    }

    std::cout << "\n=== Adaugare de elemente (<<) ===\n"; 
    A << 5 << 6; 
    std::cout << "A dupa adaugarea elementelor 5,6: " << A << "\n"; 
    
    std::cout << "\n=== Stergere de elemente (>>) ===\n"; 
    A >> 2 >> 100; 
    std::cout << "A dupa stergerea lui 2 si 100: " << A << "\n"; 
    
    std::cout << "\n=== Apartenenta ===\n"; 
    std::cout << "A contine 3? " << (A.contains(3) ? "da" : "nu") << "\n";
    std::cout << "A contine 2? " << (A.contains(2) ? "da" : "nu") << "\n"; 
    
    std::cout << "\n=== Operatii pe multimi ===\n"; 

    std::cout << "A = " << A << "\n"; 
    std::cout << "B = " << B << "\n";

    Multime<int> reuniune = A + B; 
    std::cout << "A + B (reuniune) = " << reuniune << "\n";
    
    Multime<int> intersectie = A * B; 
    std::cout << "A * B (intersectie) = " << intersectie << "\n";
    
    Multime<int> diferenta = A - B; 
    std::cout << "A - B (diferenta) = " << diferenta << "\n"; 
    
    Multime<int> diferenta2 = B - A; 
    std::cout << "B - A (diferenta) = " << diferenta2 << "\n"; 
    
    std::cout << "\n=== Operatii relationale ===\n"; 
    Multime<int> F; 
    F << 3 << 4; 
    std::cout << "F = " << F << "\n"; 
    std::cout << "B = " << B << "\n"; 

    std::cout << "F == B ? " << (F == B ? "true" : "false") << "\n"; 
    
    std::cout << "F < B ? " << (F < B ? "true" : "false") << "\n"; 
    
    std::cout << "F <= B ? " << (F <= B ? "true" : "false") << "\n";
    
    std::cout << "B > F ? " << (B > F ? "true" : "false") << "\n"; 
    
    std::cout << "B >= F ? " << (B >= F ? "true" : "false") << "\n\n"; 



    Multime<int> G; 
    Multime<int> H;
    std::cout << "G = " << G << "\n"; 
    std::cout << "H = " << H << "\n"; 

    std::cout << "G == H ? " << (G == H ? "true" : "false") << "\n"; 
    
    std::cout << "G < H ? " << (G < H ? "true" : "false") << "\n"; 
    
    std::cout << "G <= H ? " << (G <= H ? "true" : "false") << "\n";
    
    std::cout << "H > G ? " << (H > G ? "true" : "false") << "\n"; 
    
    std::cout << "H >= G ? " << (H >= G ? "true" : "false") << "\n"; 
      
     
    std::cout << "\n=== Operatii de atribuire ===\n"; 
    {
        Multime<int> I; 
        I = A;
        std::cout << "I (copiat din A) = " << I << "\n\n";
    }
    {
        Multime<int> I = A; 
        Multime<int> J; 
        J = std::move(I); 
        std::cout << "J (atribuit prin move din I) = " << J << "\n";
        std::cout << "I (dupa move) = " << I << "\n"; 
    }
    
    std::cout << "\n=== Sfarsit ===\n"; return 0;
}