#include <algorithm>
#include <cctype>
#include <ostream>
#include <sstream>
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

        // Constructor pentru conversia mai multor elemente la multimea ce contine aceste elemente
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

        // Metoda folosita doar pentru testare
        std::string getStats() const {
            return "(Size = " + std::to_string(size) + ", Capacity = " + std::to_string(capacity) + ")";
        }
    
        inline std::size_t getSize(){
            return size;
        }

        // adaos: Metoda folosita pentru a goli multimea
        void clear(){
            if(size == 0) return;

            freeElements();

            capacity = DEFAULT_CAPACITY;
            allocElements(capacity);
            size = 0;
        }

        // Afiseaza lista pe standard output
        // adaos: Pentru a evita ambiguitati in citirea de la consola, am schimbat outputul afisat pe consola pentru stringuri si caractere
        // 1. Stringurile vor contine ghilimele "" pentru a evita confuziile la parsare cu caractererele
        // 2. Caracterele speciale '{' si '}' vor fi 'escaped' cu un backslash pentru a evita confuziile cu multimi
        friend std::ostream& operator<<(std::ostream& os,const Multime<T>& multime){
            if (multime.size == 0) {os << "{}"; return os;};
           
            os << '{';
            
            // Numar elementele pentru a nu afisa o virgula in plus la final (ex: {1,2,3,})
            std::size_t count = 0;

            for(std::size_t i=0; i < multime.capacity;++i){
                Node* node = multime.elements[i];
                
                while (node != nullptr) {
                    // Stringurile vor fi afisate intre ghilimele
                    if constexpr (std::is_same_v<T, std::string>) {
                        os << '"' << node->getData() << '"';
                    } 
                    else if constexpr (std::is_same_v<T, char>) {
                        // Caracterele speciale sunt escaped
                        char ch = node->getData();
                        if (ch == '{' || ch == '}') {
                            os << '\\' << ch;
                        } else {
                            os << ch;
                        }
                    } 
                    else {
                        os << node->getData();
                    }

                    ++count;

                    // Afisam virgule doar daca nu am ajuns la ultimul element
                    if (count < multime.size) {
                        os << ", ";
                    }

                    node = node->getNext();
                }

                if (count == multime.size) break;
            }

            os << '}';
            
            return os;
        }
        
        // Permit clasei std::hash<Multime<T>> sa acceseze elementele private ale multimii pentru a genera hashul
        friend std::hash<Multime<T>>;

        // adaos: Citire de la consola
        // Forme asteptate (sunt cele create de ): 
        // '{}' - multimea vida, 
        // '{x1, x2, ..., xn}' - multime cu elemente de tip T
        // '{{x1, x2, ..., xn}, {x1, x2, ..., xn}, ... , {x1, x2, ..., xn}}' - multime cu elemente de tip Multime<T>
        // Formele acceptate sunt cele pe care le produce functia de afisare pe consola
        friend std::istream& operator>>(std::istream& is, Multime<T>& multime) {
            // Resetam multimea
            multime.clear();
            char ch = '\0';

            // Citim pana cand intalnim primul caracter non whitespace
            if (!(is >> ch)) return is;

            if (ch != '{') {
                // Formatul este incorrect
                is.setstate(std::ios::failbit);
                return is;
            }

            if(!(is >> ch)){
                is.setstate(std::ios::failbit);
                return is;
            };

            if (ch == '}') {
                // Cazul multimii vide
                return is; 
            }

            is.putback(ch);
            
            while (true) {   
                
                T element;

                // Extragem elementul in funtie de tipul T

                if constexpr (std::is_same_v<T, std::string>) {
                    // Stringurile sunt asteptate sa fie in forma: "continut"
                    if (is >> ch && ch == '"') {
                        std::string strToken;
                        // Citim pana can intalinm cealalta ghilimea
                        if (std::getline(is, strToken, '"')) {
                            element = strToken;
                        } else {
                            is.setstate(std::ios::failbit);
                        }
                    } else is.setstate(std::ios::failbit);
                }
                else if constexpr (std::is_same_v<T, char>) {
                    // Caracterele speciale trebuie tratate diferit: \{  \}
                    if (is >> ch) {
                        if (ch == '\\') {
                            // Urmatorul caracter trebuie sa fie '{' sau '}'
                            char nextCh;
                            if (is >> nextCh && (nextCh == '{' || nextCh == '}')) {
                                element = nextCh;
                            } else {
                                is.setstate(std::ios::failbit);
                            }
                        } else if (ch == '{' || ch == '}') {
                            // Caracterele special nu au fost escaped
                            is.setstate(std::ios::failbit);
                        }else{
                            element = ch;
                        }
                    }
                }else {
                    // Celalate tipuri de elemente nu sunt ambigue
                    is >> element;
                }

                // Validam ca elementul a fost parsat fara errori
                if (!is) {
                    multime.clear();
                    return is;
                }

                multime << element;
                               
                // Verificam ce urmeaza dupa element (trebuie sa fie '}' sau ',')
                if (!(is >> ch)) {
                    is.setstate(std::ios::failbit);
                    multime.clear();
                    return is;
                }
        
                if(ch == '}'){
                    break;
                } 
                
                if (ch != ','){
                    // Formatul nu este correct (e.g. '{1 2}')
                    is.setstate(std::ios::failbit);
                    multime.clear();
                    return is;
                };
                
            }
      
            return is;
        }


    private:
        // Clasa interna folosita pentru a implementa elementele listei inlantuita
        class Node{
            public:
           
            Node(const T& data, Node* next = nullptr):data(data),next(next){}
            
            Node(T&& data, Node* next = nullptr):data(std::move(data)),next(next){}
                
            ~Node(){}
            
            Node* getNext() const{
                return next;
            }

            void setNext(Node* next){
                this->next = next;
            }

            const T& getData() const{
                return data;
            }

            // Creeaza o copie a intregului lant de noduri incepand de aici
            Node* deepCopy() const{
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

        // adaos: Constructor de move 
        Point(Point&& other) {
            x = other.x;
            y = other.y; 

            // Resetam lui other la cele default
            other.x = 0;
            other.y = 0;
        };

        // adaos: Constructor de copiere;
        Point(const Point& other): x(other.x), y(other.y) {};

        // adaos: Destructor
        ~Point() {};
    
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

        friend std::ostream& operator<<(std::ostream& os, const Point& p) {
            return os << "(" << p.x << ", " << p.y << ")";
        }

        // adaos: Citire de la consola
        // Format asteptat: '(valoare_x, valoare_y)'
        // Caracterele whitespace sunt ignorate 
        friend std::istream& operator>>(std::istream& is, Point& p) {
            char ch='\0';

            // Citim pana cand intalnim primul caracter non whitespace
            if (!(is >> ch)) return is;

            if (ch != '(') {
                // Formatul este incorrect
                is.setstate(std::ios::failbit);
                return is;
            }
            
            int x = 0, y = 0;
                
            if(!(is >> x) || (!is >> ch) || ch != ','){
                // Formatul este incorrect
                is.setstate(std::ios::failbit);
                return is;
            }

            if(!(is >> y) || (!is >> ch) || ch != ')'){
                // Formatul este incorrect
                is.setstate(std::ios::failbit);
                return is;
            }

            p.x = x;
            p.y = y;
              
            return is;
        }

    private:
        int x = 0,y = 0;
};

// adaos: Clasa Complex
class Complex {
private:
    double real;
    double imag;

public:
    // Constructori

    Complex(double real = 0.0, double imag = 0.0) : real(real), imag(imag) {}
    
    Complex(const Complex& other) : real(other.real), imag(other.imag) {}

    Complex(Complex&& other) : real(other.real), imag(other.imag) {
        other.real = 0.0;
        other.imag = 0.0;
    }

    // Destructor

    ~Complex() {}

    // Operatorii de atribuire

    Complex& operator=(const Complex& other) {
        if(&other == this) return *this;

        real = other.real;
        imag = other.imag;
        
        return *this;
    }

    Complex& operator=(Complex&& other) {
        if(&other == this) return *this;

        real = other.real;
        imag = other.imag;
        other.imag = 0.0;
        other.real = 0.0;
        
        return *this;
    }

    // Operator de egalitate
    bool operator==(const Complex& other) const {
        return real == other.real && imag == other.imag;
    }

    // Minus unar
    Complex operator-() const {
        return Complex(-real, -imag);
    }

    // Operator de adunare
    Complex operator+(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }
    
    // Operator de scadere
    Complex operator-(const Complex& other) const {
        return Complex(real - other.real, imag - other.imag);
    }
    
    // Operator de inmultire
    Complex operator*(const Complex& other) const {
        return Complex(
            real * other.real - imag * other.imag,
            real * other.imag + imag * other.real
        );
    }
    
    // Operator de impartire
    Complex operator/(const Complex& other) const {
        double denominator = other.magnitudeSquared();
        if (denominator == 0.0) {
            throw std::runtime_error("Cannot divide by zero.");
        }
        
        Complex conjugate = other.getConjugate();
        Complex numerator = (*this) * conjugate;

        return Complex(
            numerator.real / denominator,
            numerator.imag / denominator
        );
    }

    Complex getConjugate() const {
        return Complex(real, -imag);
    }

    // Calculeaza magnitudinea la patrat
    double magnitudeSquared() const {
        return real * real + imag * imag;
    }

    double getReal() const { return real; }
    
    double getImag() const { return imag; }

    
    friend std::ostream& operator<<(std::ostream& os, const Complex& c) {
        // Caz 1: partea imaginara este 0, deci afisam doar partea reala
        if (c.imag == 0.0) {
            os << c.real;
            return os;
        }

        bool imagHasAbsOne = (std::abs(c.imag) == 1.0);
        
        // Caz 2: partea reala este 0, deci afisam doar partea imaginara
        // Daca valoarea absoluta a partii imaginare este 1, nu mai afisam si coeficientul sau 
        if (c.real == 0.0) {
            if (imagHasAbsOne) {
                os << ((c.imag > 0) ? "i" : "-i");
            } else {
                os << c.imag << "i";
            }
            return os;
        }

        // Caz 3: partea imaginara si cea reala sunt diferite de 0
        os << c.real;

        if (c.imag < 0.0) {
            if (imagHasAbsOne) os << "-i";
            else os << c.imag << "i";
        } else {
            if (imagHasAbsOne) os << "+i";
            else os << "+" << c.imag << "i";
        }

        return os;
    }

    // Formate acceptate: '0.3', '0.3+1.4i', '-i', '1e-5+2.3i' (notatii stiintifice)
    // Formele acceptate sunt cele pe care le produce functia de afisare pe consola
    friend std::istream& operator>>(std::istream& is, Complex& other) {
        std::string input;

        // Sarim peste caracterele whitespace de la inceput
        if (!(is >> std::ws)) {
            return is;
        }

        // Extragem numarul complex din std::cin
        while (true) {
            int nextInt = is.peek(); // Ne uitam la urmatorul caracter fara sa-l scoatem din stream
            if (nextInt == EOF) {
                break; 
            }

            char nextCh = nextInt;

            // Caracterele permise sunt: 0-9, '.', '+', '-', 'i' si 'e'
            if (std::isdigit(nextCh) || nextCh == '.' || nextCh == 'e' || 
                nextCh == '-' || nextCh == '+' || nextCh == 'i') {
                
                is.get(nextCh); // Extragem caracterul
                input += nextCh;

                // Ne oprim din citit dupa ce intalnim 'i', deoarece marcheaza sfarsitul numarului complex
                if (nextCh == 'i') {
                    break;
                }
            } else {
                // Caracterul nu este permis, ne oprim din citit 
                break;
            }
        }

        // Daca nu am reusit sa colectam niciun caracter valid, atunci inputul este invalid
        if (input.empty()) {
            is.setstate(std::ios::failbit);
            return is;
        }

        other.imag = 0.0;
        other.real = 0.0;

        // Cazurile triviale
        if(input == "0"){
            return is;
        }else if (input == "i"){
            other.imag = 1.0;
            return is;
        }else if (input == "-i"){
            other.imag = -1.0;
            return is;
        }

        try{
            std::size_t iPos = input.find('i');

            // Caz A: Inputul nu contine partea imaginara (ex: "5.5")
            if(iPos == std::string::npos){
                size_t idx;
                double real = std::stod(input, &idx);
                if (idx != input.size()) throw std::invalid_argument("Numar real invalid");
                other.real = real;
                return is;
            }

            // Caz B: 'i' este prezent si trebuie sa fie ultimul caracter
            if (iPos != input.size() - 1) {
                throw std::invalid_argument("Format invalid: 'i' trebuie sa fie ultimul caracter");
            }

            size_t idx;
            double firstNum = std::stod(input, &idx);

            // Daca std::stod a consumat toate caracterele, inseamna ca nu avem parte reala.
            // ex: "2.5i" 
            if (idx == input.size() - 1) {
                other.imag = firstNum;
                return is;
            } 
            
            other.real = firstNum;

            // Preluam coeficientul partii imaginare
            // ex: din "-3.5+5.6i" => remaining = "+5.6"
            std::string remaining = input.substr(idx, input.size() - 1 - idx);

             
            if (remaining == "+") {
                other.imag = 1.0;
            }else if (remaining == "-") {
                other.imag = -1.0;
            } else {
                size_t idxImag;
                other.imag = std::stod(remaining, &idxImag);
                if (idxImag != remaining.size()) throw std::invalid_argument("Parte imaginara invalida");
            }
            
        }catch(...){
            is.setstate(std::ios::failbit);
            other.imag = 0.0;
            other.real = 0.0;
        }

        return is;
    }
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

// adaos:
// Multimea are nevoie de o functie de hash pentru clasa Complex
namespace std {
    template <>
    struct hash<Complex> {
        std::size_t operator()(const Complex& c) const {
            // Combinam hash-urile membrilor x si y
            return std::hash<double>()(c.getImag()) ^ (std::hash<double>()(c.getReal()) << 1);
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

    std::cout << "==========================================\n";
    std::cout << "   TEST ADAOS: Multime de numere complexe \n";
    std::cout << "==========================================\n";
    {
        std::cout << "=== 1. Intializare prin simularea citirii de la consola  ===\n";
        Multime<Multime<Complex>> C;
        
        // Testam diverse forme acceptate: 0, 5+3.5i, i pur, notatie exponentiala 'e'
        std::istringstream stream(" {{}, {5+3.5i, 2-1e1i}, {-i}, {0}, {4e+2+i}} ");

        if (stream >> C) {
            std::cout << "Succes parsare: " << C << "\n";
        } else {
            std::cout << "Eroare: Parsarea a esuat pe un sir considerat valid!\n";
        }

        std::cout << "\n=== 2. Intializare prin simularea citirii de la consola folosind un sir invalid (Trebuie sa declanseze failbit) ===\n";
        std::istringstream stream_gresit(" {{1+2i}, {4+5ix}} "); // 'x' este ilegal la finalul unui numar complex
        Multime<Multime<Complex>> MultimeInvalida;

        if (!(stream_gresit >> MultimeInvalida)) {
            std::cout << "Succes test: Parser-ul a respins corect caracterul 'x'.\n";
            std::cout << "Stare multime (trebuie sa fie vida): " << MultimeInvalida << "\n";
        } else {
            std::cout << "Eroare: Parser-ul a acceptat in mod eronat un format invalid!\n";
        }

    }

    std::cout << "\n==========================================\n";
    std::cout << "   TEST ADAOS: Multime de float \n";
    std::cout << "==========================================\n";
    {
        std::cout << "=== 1. Initializare si Constructori ===\n"; 
        
        Multime<float> A; A << 1.25f << 2.5f << 3.14f << 4.0f; 
        Multime<float> B{3.14f, 4.0f, 5.75f, 6.0f}; 
        
        std::cout << "A = " << A << "\n"; 
        std::cout << "B (initializer list) =   " << B << "\n";

        std::cout << "\n=== 2. Operatii pe Multimi ===\n";
        Multime<float> reuniune = A + B;
        std::cout << "A + B (Reuniune) = " << reuniune << "\n";

        Multime<float> intersectie = A * B;
        std::cout << "A * B (Intersectie) = " << intersectie << "\n";

        Multime<float> diferenta1 = A - B;
        std::cout << "A - B (Diferenta) = " << diferenta1 << "\n";

        Multime<float> diferenta2 = B - A;
        std::cout << "B - A (Diferenta) = " << diferenta2 << "\n";

        std::cout << "\n=== 3. Operatii Relationale ===\n";
        Multime<float> S{3.14f, 4.0f};
        std::cout << "S = " << S << "\n";
        std::cout << "B = " << B << "\n";
        
        // Verificam incluziunile matematice
        std::cout << "S este submultime a lui B? (S <= B): " << (S <= B ? "Da" : "Nu") << "\n";
        std::cout << "B este superset pentru S? (B >= S): "   << (B >= S ? "Da" : "Nu") << "\n";
        std::cout << "Sunt S si B egale? (S == B): "           << (S == B ? "Da" : "Nu") << "\n";

    }

    std::cout << "\n==========================================\n";
    std::cout << "   TEST ADAOS: Citire multime de la consola (cazuri speciale) \n";
    std::cout << "==========================================\n";
    {
        std::cout << "=== 1. Multime de multimi de String ===\n";
        Multime<Multime<std::string>> S;
        // Fiecare string este delimitat strict de ghilimele. Acest lucru permite izolarea acoladelor ca date.
        std::istringstream stream(" {{}, {\"{}\"}, {\"{\"}, {\"}\"}, {\"\"}, {\"complex, string\"}} ");

        if (stream >> S) {
            std::cout << "Succes parsare: " << S << "\n";
        } else {
            std::cout << "Eroare parsare String!\n";
        }

        std::cout << "\n=== 2. Multime de multimi de char (Acolade escaped cu backslash) ===\n";
        Multime<Multime<char>> C;
        // Acoladele folosite ca date sunt escaped: \{ si \}. Cele structurale sunt curate.
        std::istringstream stream2(" {{}, {\\{,\\}}, {\\{}, {\\}}} ");

        if (stream2 >> C) {
            std::cout << "Succes parsare: " << C << "\n";
        } else {
            std::cout << "Eroare parsare char!\n";
        }

        std::cout << "\n=== 3. Multime de multimi de char invalida (Acolade unescaped in mod ilegal) ===\n";
        std::istringstream stream3(" {{}, {\\{,\\}}, {\\{}, {{}} "); // Ultima submultime contine un '{' unescaped 
        Multime<Multime<char>> MultimeInvalida;

        if (!(stream3 >> MultimeInvalida)) {
            std::cout << "Succes test! Parser-ul a respins corect caracterul unescaped '{'\n";
            std::cout << "Stare multime (trebuie sa fie vida):" << MultimeInvalida << "\n";
        } else {
            std::cout << "Eroare: Parser-ul a acceptat in mod eronat un format invalid!\n";
            std::cout << MultimeInvalida << "\n";
        }

    }
   
    std::cout << "\n==========================================\n";
    std::cout << "   SFARSIT TEST\n";
    std::cout << "==========================================\n";
}