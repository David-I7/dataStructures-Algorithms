#include <algorithm>
#include <ostream>
#include <string>
#include <utility>
#include <iostream>
#include <initializer_list>

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

        // Constructor pentru conversia mai multor elemente la multimea ce contine aceste elementele 
        Multime(std::initializer_list<T> list):capacity(DEFAULT_CAPACITY){
            allocElements(capacity);
            for(const T& el: list){
                *this << el;
            }
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

            // Lasam obiectul 'other' intr-o stare valida, dar goala, pentru a evita crash-uri
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
    
        // Adauga un element in multime cand este lvalue
        Multime<T>& operator<<(const T& el){
            return addImpl(el);
        }

        // Adauga un element in multime cand este rvalue
        Multime<T>& operator<<(T&& el){
            return addImpl(std::move(el));
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

        // Metoda folosita doar pentru testare
        std::string getStats() const {
            return "(Size = " + std::to_string(size) + ", Capacity = " + std::to_string(capacity) + ")";
        }

        inline std::size_t getSize(){
            return size;
        }

        // Afiseaza lista pe standard output
        friend std::ostream& operator<<(std::ostream& os,const Multime<T>& multime){
            if (multime.size == 0) {os << "{}"; return os;};
           
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
        
        // Permit clasei std::hash<Multime<T>> sa acceseze elementele private ale multimii pentru a genera hashul
        friend std::hash<Multime<T>>;
        
        // control granular
        //friend std::size_t std::hash<Multime<T>>::operator()(const Multime<T>& multime) const;

    private:
        // Clasa interna folosita pentru a implementa elementele listei inlantuita
        class Node{
            public:
           
            // Member initializer list previne compilatorul sa dea valori default
            // variabilelor declarate in clasa. 
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

        // Adauga un element in multime.
        // Cu std::forward<U>(el), prezervam tipului referintei elementului 'el'.
        // Altfel, un rvalue transmis catre functie ar deveni lvalue in interiorul functiei.  
        template< typename U>
        Multime<T>& addImpl(U&& el){
            std::size_t h = hashIndex(el,capacity);
            Node* node = elements[h];
            
            if (node == nullptr) {
                elements[h] = new Node(std::forward<U>(el));
                ++size;
            }else{
                // Daca nu exista deja, il adaugam in O(1) direct la capul listei
                if (node->find(el) == nullptr){
                    elements[h] = new Node(std::forward<U>(el),node);
                    ++size;
                }
            };

            checkResize();
            return *this;
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

// Clasa 'multime' are nevoie de o functie de hash in caz ca vrem sa avem o multime de multimi (Multime<Multime<T>>)
namespace std {
    template<typename T>
    struct hash<Multime<T>>{
        std::size_t operator()(const Multime<T>& multime) const{
            std::size_t seed = 0;
            for(std::size_t i = 0; i < multime.size;++i){
                auto node = multime.elements[i];
                
                while(node != nullptr){
                    // XOR este comutativ, deci nu conteaza ordinea in care calculam hashul
                    // ex: ((1 ^ 2 )^ 3) = (1 ^ (2 ^ 3)) = 0
                    seed ^= std::hash<T>{}(node->getData());
                    node= node->getNext();
                }
            }
            return seed;
        }
    };
}

// ==================================================================
// Testare
// ==================================================================

// Clasa folosita pentru test
class Point{
    public:
        Point(int x = 0, int y = 0): x(x),y(y){};
    
        inline int getX() const {
            return x;
        }
    
        inline int getY() const {
            return y;
        }

        inline void setX(int x) {
            this->x = x;
        }

        inline void setY(int y) {
            this->y = y;
        }

        // Multimea are nevoie de operatorul == pentru a verifica duplicatele
        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }

        // NU este e functie asociata clasei, chiar daca este declarata in interiorul ei.
        friend std::ostream& operator<<(std::ostream& os, const Point& p) {
            return os << "(" << p.x << ", " << p.y << ")";
        }

    private:
        int x = 0,y = 0;
};

// Multimea are nevoie de o functie de hash pentru clasa Point
namespace std {
    template <>
    struct hash<Point> {
        std::size_t operator()(const Point& p) const {
            // Combinam hash-urile membrilor x si y
            return std::hash<int>()(p.getX()) ^ (std::hash<int>()(p.getY()) << 1);
        }
    };
}

int main(){
    std::cout << "==========================================\n";
    std::cout << "   TEST: Multime de int \n";
    std::cout << "==========================================\n";
    {
        std::cout << "=== 1. Initializare si Constructori ===\n"; 
        
        Multime<int> A; A << 1 << 2 << 3 << 4; 
        Multime<int> B{3,4,5,6}; 
        
        std::cout << "A = " << A << "\n"; 
        std::cout << "B (initializer list)=  " << B << "\n"; 
        
        // Verificam alocarea initiala cand se ofera direct un element
        Multime<int> C(10); 
        std::cout << "C (singur element) = " << C << "\n"; 
        
        // Testam deep copy-ul (modificarea lui D nu trebuie sa afecteze pe A)
        Multime<int> D = A; 
        std::cout << "D (copie a lui A) = " << D << "\n";

        D << 0;
        std::cout << "D (dupa modificare) = "  << D << "\n";
        std::cout << "A (dupa modificarea din D) = " << A <<"\n";

        // Testam move constructor-ul (D trebuie sa ramana intr-o stare goala, dar valida)
        Multime<int> E = std::move(D); 
        std::cout << "E (mutat din D) = " << E << "\n"; 
        std::cout << "D (dupa move, trebuie sa fie {}) = " << D << "\n"; 

        std::cout << "\n=== 2. Testare stare 'Moved-From' ===\n";
        // Asiguram faptul ca un obiect mutat poate fi refolosit fara sa crape programul
        D << 100 << 200; 
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
        Multime<int> S{3,4};
        std::cout << "S = " << S << "\n";
        std::cout << "B = " << B << "\n";
        
        // Verificam incluziunile matematice
        std::cout << "S este submultime a lui B? (S <= B): " << (S <= B ? "Da" : "Nu") << "\n";
        std::cout << "B este superset pentru S? (B >= S): "   << (B >= S ? "Da" : "Nu") << "\n";
        std::cout << "Sunt S si B egale? (S == B): "           << (S == B ? "Da" : "Nu") << "\n\n";

        std::cout << "=== 5. Test de Stres (Coliziuni si Resize) ===\n";
        Multime<int> StressSet;
        
        std::cout << "Initial: " << StressSet.getStats() << "\n";

        // Fortam extinderea capacitatii (Rehashing in sus)
        for(int i = 0; i < 10000; i++) {
            StressSet << (i * 10);
        }
        
        std::cout << "Dupa adaugare 10000 elemente:\n";
        std::cout << "Stats: " << StressSet.getStats() << "\n";

        // Fortam micsorarea capacitatii (Rehashing in jos)
        for(int i = 0; i < 9500; i++) {
            StressSet >> (i * 10);
        }
        
        std::cout << "Dupa stergere 9500 elemente:\n";
        std::cout << "Stats: " << StressSet.getStats() << "\n";

        std::cout << "\n=== 6. Testare copy si move assignment ===\n";
        Multime<int> Original{1,2,3};
        Multime<int> Copie; 
        Multime<int> Moved;

        // Atribuire prin copiere (Deep Copy)
        Copie = Original;
        std::cout << "Original: " << Original << "\n";

        // Modificam copia
        Copie << 4 << 5; 
        std::cout << "Copie (modificata, nu afecteaza Original): " << Copie << "\n";
        std::cout << "Original (dupa modificarea copiei): " << Original << "\n";

        // Atribuire prin move 
        Moved = std::move(Original);
        
        // Atribuire prin move (Transfer de resurse)
        std::cout << "Moved (preia datele din Original): " << Moved << "\n";
        std::cout << "Original (golit dupa mutare): " << Original << "\n\n";
    }

    std::cout << "==========================================\n";
    std::cout << "   TEST: Multime de point \n";
    std::cout << "==========================================\n";
    // Acest test verifica daca multimea foloseste corect specializarea 
    // std::hash<Point> si operatorul '==' definit de utilizator.
    {
        std::cout << "=== 1. Initializare si Constructori ===\n"; 
        
        Multime<Point> A; A << Point{0, 0} << Point{1, 1} << Point{5, 5};
        Multime<Point> B{Point{1, 2} , Point{-1, -1}}; 
        
        std::cout << "A = " << A << "\n"; 
        std::cout << "B (initializer list)=  " << B << "\n"; 
        
        std::cout << "\n=== 2. Operatii pe Multimi ===\n";
        Multime<Point> reuniune = A + B;
        std::cout << "A + B (Reuniune) = " << reuniune << "\n";

        Multime<Point> intersectie = A * B;
        std::cout << "A * B (Intersectie) = " << intersectie << "\n";

        Multime<Point> diferenta1 = A - B;
        std::cout << "A - B (Diferenta) = " << diferenta1 << "\n";

        Multime<Point> diferenta2 = B - A;
        std::cout << "B - A (Diferenta) = " << diferenta2 << "\n";

        std::cout << "\n=== 3. Operatii Relationale ===\n";
        Multime<Point> S{Point{0, 0}};
        std::cout << "S = " << S << "\n";
        std::cout << "B = " << B << "\n";
        
        std::cout << "S este submultime a lui B? (S <= B): " << (S <= B ? "Da" : "Nu") << "\n";
        std::cout << "B este superset pentru S? (B >= S): "   << (B >= S ? "Da" : "Nu") << "\n";
        std::cout << "Sunt S si B egale? (S == B): "           << (S == B ? "Da" : "Nu") << "\n";

        std::cout << "\n=== 4. Test de Stres (Coliziuni si Resize) ===\n";
        Multime<Point> StressSet;
        
        std::cout << "Initial: " << StressSet.getStats() << "\n";

        for(int i = 0; i < 1000; i++) {
            StressSet << Point{(i * 10),(i*10)};
        }
        
        std::cout << "Dupa adaugare 1000 elemente:\n";
        std::cout << "Stats: " << StressSet.getStats() << "\n";

        for(int i = 0; i < 980; i++) {
            StressSet >> Point{(i * 10),(i*10)};
        }
        
        std::cout << "Dupa stergere 980 elemente:\n";
        std::cout << "Stats: " << StressSet.getStats() << "\n\n";
    }

    std::cout << "==========================================\n";
    std::cout << "   TEST: Multime de multimi \n";
    std::cout << "==========================================\n";
    // Acest test verifica daca clasa poate sa hasheze 
    // si sa compare propriul tip de date recursiv.
    {

        std::cout << "=== 1. Initializare si Constructori ===\n"; 
        
        Multime<Multime<int>> A; A << Multime<int>{} << Multime<int>{ 0} << Multime<int>{1, 2,3} << Multime<int>{5,6};
        Multime<Multime<int>> B{Multime<int>{},Multime<int>{1, 2,3} , Multime<int>{0, 1}}; 
        
        std::cout << "A = " << A << "\n"; 
        std::cout << "B (initializer list)=  " << B << "\n"; 
        
        std::cout << "\n=== 2. Operatii pe Multimi ===\n";
        Multime<Multime<int>> reuniune = A + B;
        std::cout << "A + B (Reuniune) = " << reuniune << "\n";

        Multime<Multime<int>> intersectie = A * B;
        std::cout << "A * B (Intersectie) = " << intersectie << "\n";

        Multime<Multime<int>> diferenta1 = A - B;
        std::cout << "A - B (Diferenta) = " << diferenta1 << "\n";

        Multime<Multime<int>> diferenta2 = B - A;
        std::cout << "B - A (Diferenta) = " << diferenta2 << "\n";

        std::cout << "\n=== 3. Operatii Relationale ===\n";
        Multime<Multime<int>> S{Multime<int>{},Multime<int>{1, 2,3} , Multime<int>{0, 1}};
        std::cout << "S = " << S << "\n";
        std::cout << "B = " << B << "\n";
        
        std::cout << "S este submultime a lui B? (S <= B): " << (S <= B ? "Da" : "Nu") << "\n";
        std::cout << "B este superset pentru S? (B >= S): "   << (B >= S ? "Da" : "Nu") << "\n";
        std::cout << "Sunt S si B egale? (S == B): "           << (S == B ? "Da" : "Nu") << "\n";

        std::cout << "\n=== 4. Test de Stres (Coliziuni si Resize) ===\n";
        Multime<Multime<int>> StressSet;
        
        std::cout << "Initial: " << StressSet.getStats() << "\n";

        for(int i = 0; i < 1000; i++) {
            StressSet << Multime<int>{(i * 10)};
        }
        
        std::cout << "Dupa adaugare 1000 elemente:\n";
        std::cout << "Stats: " << StressSet.getStats() << "\n";

        for(int i = 0; i < 900; i++) {
            StressSet >> Multime<int>{(i * 10)};
        }
        
        std::cout << "Dupa stergere 900 elemente:\n";
        std::cout << "Stats: " << StressSet.getStats() << "\n\n";
    }
    
    std::cout << "==========================================\n";
    std::cout << "   TEST: Multime ce contine pointeri \n";
    std::cout << "==========================================\n";
    // Acest test demonstreaza ca multimea distruge doar resursele proprii,
    // nu si cele alocate de utilizator.
    {
     
        std::cout << "Creez doi pointeri distincti ce pointeaza catre valoarea 42 si ii adaug in multime\n";
        int* ptr1 = new int(42);
        int* ptr2 = new int(42);
        
        std::cout << "ptr1 = " << ptr1 <<  ", *ptr1 == 42" << "\n";
        std::cout << "ptr2 = " << ptr2 <<  ", *ptr2 == 42" << "\n\n";
        
        {
            Multime<int*> SetPointeri;
            SetPointeri << ptr1 << ptr2;
            std::cout << "SetPointeri = " << SetPointeri << "\n";
            std::cout << "SetPointeri (contine pointerul ptr1?) : " << (SetPointeri.contains(ptr1) ? "Da" : "Nu") << "\n";
            int* ptr3 = new int(42);
            std::cout << "Creez pointerul:" << " ptr3 = " << ptr3 <<  ", *ptr3 == 42" << "\n";
            std::cout << "SetPointeri (contine pointerul ptr3?) : " << (SetPointeri.contains(ptr3) ? "Da" : "Nu") << "\n";
            std::cout << "SetPointeri iese din scop si se apeleaza destructorul...\n\n";
            delete ptr3;
        }

        std::cout << "Multimea a fost distrusa, mai sunt valizi pointerii ptr1 si ptr2? \n";
        std::cout << "ptr1 = " << ptr1 <<  ", *ptr1 == " << *ptr1 << " (DA)" << "\n";
        std::cout << "ptr2 = " << ptr2 <<  ", *ptr2 == " << *ptr2 << " (DA)" << "\n\n";

        delete  ptr1;
        delete  ptr2;
    }

    std::cout << "\n==========================================\n";
    std::cout << "   SFARSIT TEST\n";
    std::cout << "==========================================\n";
}