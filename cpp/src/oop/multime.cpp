#include <algorithm>
#include <ostream>
#include <utility>
#include <iostream>

// Multimea este construita ca un array de liste inlantuite (Separate Chaining Hash Set)
// Exemplu: [Node*, nullptr, Node*, nullptr, ...]
// Unde:
// - Node* pointeaza catre capul unei liste inlantuite in caz de coliziuni (acelasi hash).
// - nullptr inseamna ca niciun element din multime nu a fost mapat la acel index.

template <typename T>
class Multime{
   
    public:
        ~Multime(){
            freeElements();
        }

        // Constructor default
        Multime(): capacity(DEFAULT_CAPACITY){
            allocElements(capacity);
        }

        // Conversia unui element la multimea cu un singur element
        Multime(const T& element): capacity(DEFAULT_CAPACITY){
            allocElements(capacity);
            std::size_t h = hashIndex(element, capacity);
            elements[h] = new Node(element);
            size++;
        }
        
        // Conversia unui element la multimea cu un singur element (folosind move semantics)
        Multime(T&& element): capacity(DEFAULT_CAPACITY){
            allocElements(capacity);
            std::size_t h = hashIndex(element, capacity);
            elements[h] = new Node(std::move(element));
            size++;
        }

        // Constructor de copiere (Deep Copy)
        Multime(const Multime<T>&other){
            allocElements(other.capacity);
            if (other.size > 0){
                copyElements(other.elements,elements, capacity);
            }
            size = other.size;
        }
        
        // Constructor move (Transfer de resurse fara alocari noi)
        Multime(Multime<T>&&other){
            elements = other.elements;
            capacity = other.capacity;
            size = other.size;

            // Lasam obiectul 'other' intr-o stare valida, dar goala, pentru a evita crash-uri (modulo 0)
            other.capacity = DEFAULT_CAPACITY;
            other.allocElements(other.capacity);
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

            // Resetam sursa la o stare de baza valida
            other.capacity = DEFAULT_CAPACITY;
            other.allocElements(other.capacity);
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
                // Daca nu exista deja, il adaugam in O(1) direct la capul listei
                if (node->find(el) == nullptr){
                    elements[h] = new Node(el,node);
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
          
                if (node->getData() == el){
                    // Elementul este chiar capul listei inlantuite
                    elements[h] = node->getNext();
                    node->setNext(nullptr);
                    delete node;
                }else{
              
                    // Gasesc nodul care pointeaza catre nodul ce contine elementul pentru a restaura legaturile listei inlantuite
                    Node* before = node->findBefore(el);

                    // Elementul nu se afla in multime
                    if (before == nullptr) return *this;

                    // Restaurez legaturile listei
                    // Before nu o sa fie niciodata coada listei
                    Node* toDelete = before->getNext();
                    before->setNext(toDelete->getNext());
                    toDelete->setNext(nullptr);
                    delete toDelete;
                }   
            }
            
            --size;
            checkResize();
            return *this;
        }

        // Multimile sunt egale daca au aceleasi numar elemente (size) si sunt egale
        bool operator==(const Multime<T>& other) const{
            if (other.size != size) return false;
 
            return containsAll(*this, other);
        }

        // Returneaza adevarat daca partea stanga a operatiei este o submultime stricta a partii drepti 
        bool operator<(const Multime<T>& other) const{
            if (size >= other.size) return false;

            return containsAll(*this, other);
        }
        
        // Returneaza adevarat daca partea stanga a operatiei este o submultime a partii drepti 
        bool operator<=(const Multime<T>& other) const {
            if (size > other.size) return false;

            return containsAll(*this, other);
        }
        
        // Returneaza adevarat daca partea stanga a operatiei este un superset strict a partii drepti 
        bool operator>(const Multime<T>& other) const{
            if (other.size >= size) return false;

            return containsAll(other, *this);
        }
        
        // Returneaza adevarat daca partea stanga a operatiei este un superset a partii drepti 
        bool operator>=(const Multime<T>& other) const{
            if (other.size > size) return false;
            
            return containsAll(other, *this);
        }

        // Reuniune pe multimi
        Multime<T> operator+(const Multime<T>& other) const{
            // Cazurile triviale
            if (other.size == 0 && size == 0) return Multime<T>();
            else if (other.size == 0) return Multime<T>(*this);
            else if (size == 0) return Multime<T>(other);

            // Copiez multimea other
            Multime<T> reuniune = other;

            // Adaugam elementele din 'this'. Operatorul << garanteaza unicitatea.
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
        Multime<T> operator*(const Multime<T>& other) const{
            Multime<T> intersectie;

            // Optimizare: iteram prin multimea mai mica
            const Multime<T>& minSet = (size <= other.size) ? *this : other;
            const Multime<T>& maxSet = (size <= other.size) ? other : *this;
            
            for (std::size_t i = 0; i < minSet.capacity; ++i) {
                Node* node = minSet.elements[i];
                while (node != nullptr) {
                    if (maxSet.contains(node->getData())) {
                        intersectie << node->getData();
                    }
                    node = node->getNext();
                }
            }


            return intersectie;
        }

        // Diferenta pe multimi. Partea stanga mai putin partea dreapta
         Multime<T> operator-(const Multime<T>& other) const{
            // Cazurile triviale
            if (other.size == 0) return Multime<T>(*this);
            else if (size == 0) return Multime<T>();

            Multime<T> diferenta;

            // Adaug elementele din multimea 'this' care nu se gasesc in multimea 'other'
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

        // Verifica apartenenta in multime 
        bool contains(const T& el) const{
            if (size == 0) return false;

            std::size_t h = hashIndex(el, capacity);

            Node* node = elements[h];
           
            return node && node->find(el) != nullptr;
        }

        std::string getStats() const {
        return "(Size = " + std::to_string(size) + ", Capacity = " + std::to_string(capacity) + ")";
    }

        inline std::size_t getSize(){
            return size;
        }

        // Afiseaza lista pe standard output
        friend std::ostream& operator<<(std::ostream& os,const Multime<T>& multime){
            if (multime.size == 0) os << "{}";
           
            os << '{';
            
            // Numar elementele pentru a nu afisa o virgula in plus la final (ex: {1,2,3,})
            std::size_t count = 0;

            for(std::size_t i=0; i < multime.capacity;++i){
                Node* node = multime.elements[i];
                
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
            
            return os;
        }


    private:
        // Clasa interna folosita pentru a implementa elementele listei inlantuita
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

            // Creeaza o copie a intregului lant de noduri incepand de aici
            // Puteam face acest lucru si prin copy constructor sau copy assignment, dar am preferat sa folosesc o metoda pentru lizibilitate
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

            // Gaseste nodul din lista inlantuita ce contine elementul 'el'
            Node* find(const T& el){
                Node* root = this;
                while (root != nullptr) {
                    if (root->getData() == el) return root;
                    root = root->getNext();
                }

                return root;
            }

            // Gaseste nodul din lista inlantuita ce pointeaza catre nodul ce contine elementul 'el'
            Node* findBefore(const T& el){
                Node* root = this;
                while (root->getNext() != nullptr) {
                    if (root->getNext()->getData()== el) return root;
                    root = root->getNext();
                }

                return nullptr;
            }

            private:
                T data;
                Node* next;
        };

        std::size_t capacity=0, size=0;
        Node** elements = nullptr;
        static const std::size_t DEFAULT_CAPACITY = 10;
       
        inline std::size_t hashIndex(const T& el, std::size_t capacity) const{
            return std::hash<T>{}(el) % capacity;
        }

        // Returneaza true daca toate elementele din A se gasesc in B
        bool containsAll(const Multime<T>& a, const Multime<T>& b) const{
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

        // Dezaloca toata memoria structurii
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

        // Aloca un array de pointeri gol 
        void allocElements(std::size_t capacity){
            // Initializeaza memoria cu nullptr
            elements = new Node*[capacity]();
            this->capacity = capacity;
        }

        // Realoca si re-hasheaza toate elementele in noul spatiu
        void reallocElements(std::size_t newCapacity){
            Node** buff = new Node*[newCapacity]();
        
            moveElements(elements, capacity, buff, newCapacity);
            
            // Stergem doar array-ul vechi, nodurile au fost mutate
            delete[] elements; 
            elements = buff;
            capacity = newCapacity;
        }

        // Functie folosita pentru Deep Copy
        void copyElements(Node** fromBuf,Node** toBuf,std::size_t capacity){
            for(std::size_t i = 0; i < capacity; ++i){
                
                Node* node = fromBuf[i];
                if (node == nullptr) continue;

                // Hashul este acelasi pentru ca lungimea (capacity) este aceeasi
                std::size_t h = i;
                toBuf[h] = node->deepCopy();
            }
        }

        // Transfera (fara deep copy) nodurile de la un array vechi catre unul nou. Folosit la resize.
        void moveElements(Node** oldBuf,std::size_t oldCapacity,Node** newBuf,std::size_t newCapacity){
            for(std::size_t i = 0; i < oldCapacity; ++i){
                
                Node* node = oldBuf[i];
                if (node == nullptr) continue;

                do{
                    // Calculez noul hash
                    std::size_t h = hashIndex(node->getData(), newCapacity);
                    
                    Node* next = node->getNext();
                    // Inserez nodul in memoria noua alocata la capul listei
                    Node* head = newBuf[h];
                    node->setNext(head);
                    newBuf[h] = node;
                    node = next;
                
                }while (node != nullptr);

                oldBuf[i] = nullptr;
            }
        }

        // Verifica daca capacity este 0 inainte de a adauga un element nou in multime
        // Putem ajunge in aceasta stare daca am facut move de la o multime la alta.
        // Move lasa multimea sursa in starea capacity = 0, size = 0, elements = nullptr
        void checkAlloc(){
            if (capacity == 0){
                allocElements(DEFAULT_CAPACITY);
                size =0;
            }
        }

        // Verfica daca este necesar sa adaug sau sa reduc din capacity.
        void checkResize(){
            if (size >= 2 * capacity){
                // S-a atins pragul superior, dublam spatiul
                reallocElements(2 * capacity);
            }else if (capacity > DEFAULT_CAPACITY && size <= capacity / 2){
                // Prea mult spatiu gol, injumatatim spatiul
                std::size_t minCap = DEFAULT_CAPACITY;
                reallocElements(std::max(capacity / 2,minCap));
            }
        }
};

// =========================================================
// Testare
// =========================================================

int main(){
    std::cout << "=== 1. Initializare si Constructori ===\n"; 
    
    Multime<int> A; A << 1 << 2 << 3 << 4; 
    Multime<int> B; B << 3 << 4 << 5 << 6; 
    
    std::cout << "A = " << A << "\n"; 
    std::cout << "B = " << B << "\n"; 
    
    // Constructor cu un singur element 
    Multime<int> C(10); 
    std::cout << "C (singur element) = " << C << "\n"; 
    
    // Constructor de copiere
    Multime<int> D = A; 
    std::cout << "D (copie a lui A) = " << D << "\n"; 
   
    // Constructor move 
    Multime<int> E = std::move(D); 
    std::cout << "E (mutat din D) = " << E << "\n"; 
    std::cout << "D (dupa move, trebuie sa fie {}) = " << D << "\n"; 

    std::cout << "\n=== 2. Testare stare 'Moved-From' ===\n";
    D << 100 << 200; // Adaugam in obiectul care a fost moved-from (sa vedem daca mai e valid)
    std::cout << "D (dupa adaugare) = " << D << "\n";
    D >> 100;
    std::cout << "D (contine 200?) = " << (D.contains(200) ? "Da" : "Nu") << "\n";

    std::cout << "\n=== 3. Operatii pe Multimi ===\n";
    Multime<int> reuniune = A + B;
    std::cout << "A + B (Reuniune) = " << reuniune << "\n";

    Multime<int> intersectie = A * B;
    std::cout << "A * B (Intersectie) = " << intersectie << "\n";

    Multime<int> diferenta1 = A - B;
    std::cout << "A - B (Diferenta) = " << diferenta1 << "\n";

    Multime<int> diferenta2 = B - A;
    std::cout << "B - A (Diferenta) = " << diferenta2 << "\n";

    std::cout << "\n=== 4. Operatii Relationale ===\n";
    Multime<int> Sub; Sub << 3 << 4;
    std::cout << "Sub = " << Sub << "\n";
    std::cout << "B = " << B << "\n";
    
    std::cout << "Sub este submultime a lui B? (Sub <= B): " << (Sub <= B ? "Da" : "Nu") << "\n";
    std::cout << "B este superset pentru Sub? (B >= Sub): "   << (B >= Sub ? "Da" : "Nu") << "\n";
    std::cout << "Sunt Sub si B egale? (Sub == B): "           << (Sub == B ? "Da" : "Nu") << "\n\n";

    Multime<int> Vid1;
    Multime<int> Vid2;
    std::cout << "Vid1 = " << Vid1 << "\n";
    std::cout << "Vid2 = " << Vid2 << "\n";

    std::cout << "Vid1 este submultime a lui Vid2? (Vid1 <= Vid2): " << (Vid1 <= Vid2 ? "Da" : "Nu") << "\n";
    std::cout << "Vid1 este submultime stricta a lui Vid2? (Vid1 < Vid2): " << (Vid1 < Vid2 ? "Da" : "Nu") << "\n";
    std::cout << "Sunt Vid1 si Vid2 egale? (Vid1 == Vid2): "           << (Vid1 == Vid2 ? "Da" : "Nu") << "\n\n";
    
    std::cout << "\n=== 5. Test de Stres (Coliziuni si Resize) ===\n";
   Multime<int> StressSet;
    
    std::cout << "Initial: " << StressSet.getStats() << "\n";

    for(int i = 0; i < 100; i++) {
        StressSet << (i * 10);
    }
    
    std::cout << "Dupa adaugare 50 elemente:\n";
    std::cout << "Stats: " << StressSet.getStats() << "\n";
    std::cout << "Continut: " << StressSet << "\n\n";

    for(int i = 0; i < 95; i++) {
        StressSet >> (i * 10);
    }
    
    std::cout << "Dupa stergere 45 elemente:\n";
    std::cout << "Stats: " << StressSet.getStats() << "\n";
    std::cout << "Continut: " << StressSet << "\n\n";

    std::cout << "=== 6. Testare Deep Copy si Self-Assignment ===\n";
    Multime<int> Original; Original << 1 << 2 << 3;
    Multime<int> Copie = Original;
    
    // Modificam copia
    Copie << 4 << 5; 
    std::cout << "Original: " << Original << "\n";
    std::cout << "Copie (modificata): " << Copie << "\n";
    
    // Test Self-Assignment
    Original = Original; 
    std::cout << "Original (dupa Original = Original): " << Original << "\n";
  

    std::cout << "\n=== 7. Utilizare cu Tipuri Complexe (std::string) ===\n";
    
    Multime<std::string> Nume1; Nume1 << "Ion" << "Maria" << "Ana";
    Multime<std::string> Nume2; Nume2 << "Maria" << "Vasile";
    std::cout << "Nume1 = " << Nume1 << "\n";
    std::cout << "Nume2 = " << Nume2 << "\n";
    std::cout << "Reuniune = " << (Nume1 + Nume2) << "\n";
    std::cout << "Intersectie = " << (Nume1 * Nume2) << "\n";
    std::cout << "Diferenta (Nume1 - Nume2) = " << (Nume1 - Nume2) << "\n";
    std::cout << "Diferenta (Nume2 - Nume1) = " << (Nume2 - Nume1) << "\n";

    std::cout << "\n=== 7.1 Test Stres cu Tipuri Complexe ===\n";
    Multime<std::string> BigStrings;
    for(int i = 0; i < 50; i++) {
        BigStrings << ("Item_nr_" + std::to_string(i));
    }
    
    std::cout << "Dupa adaugare 50 elemente:\n";
    std::cout << "Stats: " << BigStrings.getStats() << "\n";
    std::cout << "Continut: " << BigStrings << "\n\n";
              
    for(int i = 0; i < 45; i++) {
        BigStrings >> ("Item_nr_" + std::to_string(i));
    }
    
    std::cout << "Dupa stergere 45 elemente:\n";
    std::cout << "Stats: " << BigStrings.getStats() << "\n";
    std::cout << "Continut: " << BigStrings << "\n\n";
   
    std::cout << "\n=== Sfarsit ===\n"; return 0;
}