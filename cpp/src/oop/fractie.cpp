#include <cstdlib>
#include <sstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string.h>

class GaussianInteger{
    public:

    GaussianInteger(int real, int imag): real(real),imag(imag){}
    
    GaussianInteger(int real): real(real),imag(0){}

    ~GaussianInteger(){}

    GaussianInteger(const GaussianInteger& other): real(other.real),imag(other.imag){}
   
    GaussianInteger(GaussianInteger&& other): real(other.real),imag(other.imag){}

    void operator=(const GaussianInteger& other) {
        imag = other.imag ;
        real = other.real;
    }

    void operator=(GaussianInteger&& other) {
        imag = other.imag;
        real = other.real;
    }

    bool operator==(const GaussianInteger& other) const{
        return imag == other.imag && real == other.real;
    }

    GaussianInteger operator+(const GaussianInteger& other) const{
        return GaussianInteger(
            real + other.real,
            imag + other.imag
        );
    }
    
    GaussianInteger operator-(const GaussianInteger& other) const{
        return GaussianInteger(
            real - other.real,
            imag - other.imag
        );
    }
    
    GaussianInteger operator-() const{
        return GaussianInteger(
            -real,
            -imag
        );
    }
    
    GaussianInteger operator*(const GaussianInteger& other) const{
        return GaussianInteger(
            real * other.real - imag * other.imag,
            real * other.imag + imag * other.real
        );
    }
    
    GaussianInteger operator/(const GaussianInteger& other) const{
        if (other == GaussianInteger(0)) throw std::overflow_error("Cannot divide by 0");
        
        int numarator = other.conjugat();
        GaussianInteger conjugat = other.getConjugat();
        GaussianInteger numitor = conjugat * (*this);

        return GaussianInteger(
            numitor.real / numarator,
            numitor.imag / numarator
        );
    }

    GaussianInteger getConjugat() const{
        return GaussianInteger(real,-imag);
    }

    int conjugat() const{
        return real * real + imag * imag;
    }

    friend std::ostream& operator<<(std::ostream& os, const GaussianInteger& g) {
        // caz 1: avem doar partea reala a numarului sau chiar 0
        if (g.imag == 0) {
            os << g.real;
            return os;
        }

        bool imagHasAbsOne = g.imag == 1 || g.imag == -1;
        
        // caz 2: avem doar partea imaginara a numarului
        if (g.real == 0){
            // caz 2.1: partea imaginara este 1 sau -1
            if (imagHasAbsOne){
                os << ((g.imag == 1) ? "-i" : "i");
            }else{
                // caz 2.2: partea imaginara nu este 1 sau -1
                os << g.imag << "i";
            }

            return os;
        }

        // caz 3: avem parte imaginara si reala
        os << g.real;

        // caz 3.1: partea imaginara este negativa
        if (g.imag < 0) {
            if (imagHasAbsOne) os << "-";
            else os << g.imag;
        }
        // caz 3.2: partea imaginara este postiva
        else {
            if (imagHasAbsOne) os << "+";
            else os << "+" << g.imag;
        }

        os << "i";

        return os;
    }

    friend std::istream& operator>>(std::istream& is,GaussianInteger& other){
        // Forma numar 1+3i sau 1-3i sau 1 sau -3i sau 0;

        std::string s;
        is >> s;
        GaussianInteger integer = GaussianInteger::parseGaussian(s);
        
        other = integer;

        return is;
    }

    std::string toString(){
        std::ostringstream oss;

        // caz 1: avem doar partea reala a numarului sau chiar 0
        if (imag == 0) {
            oss << real;
            return oss.str();
        }

        bool imagHasAbsOne = imag == 1 || imag == -1;
        
        // caz 2: avem doar partea imaginara a numarului
        if (real == 0){
            // caz 2.1: partea imaginara este 1 sau -1
            if (imagHasAbsOne){
                oss << ((imag == 1) ? "-i" : "i");
            }else{
                // caz 2.2: partea imaginara nu este 1 sau -1
                oss << imag << "i";
            }

            return oss.str();
        }

        // caz 3: avem parte imaginara si reala
        oss << real;

        // caz 3.1: partea imaginara este negativa
        if (imag < 0) {
            if (imagHasAbsOne) oss << "-";
            else oss << imag;
        }
        // caz 3.2: partea imaginara este postiva
        else {
            if (imagHasAbsOne) oss << "+";
            else oss << "+" << imag;
        }

        oss << "i";

        return oss.str();
    }

    static GaussianInteger parseGaussian(const std::string& s) {
            int real = 0, imag = 0;

            size_t i_pos = s.find('i');

            // Case 1: avem doar partea reala
            if (i_pos == std::string::npos) {
                real = std::stoi(s);
                return GaussianInteger(real, 0);
            }

            // Sterg i
            std::string without_i = s.substr(0, i_pos);

            // Gasesc ultimul + sau -
            size_t split = std::string::npos;
            for (size_t i = 1; i < without_i.size(); ++i) {
                if (without_i[i] == '+' || without_i[i] == '-') {
                    split = i;
                }
            }

            // Case 2: avem doar partea reala (e.g., "i", "-i", "3i")
            if (split == std::string::npos) {
                if (without_i == "" || without_i == "+") imag = 1;
                else if (without_i == "-") imag = -1;
                else imag = std::stoi(without_i);
                return GaussianInteger(0, imag);
            }

            // Case 3: avem partea reala si imaginara
            real = std::stoi(without_i.substr(0, split));
            std::string imag_part = without_i.substr(split);

            if (imag_part == "+" || imag_part == "") imag = 1;
            else if (imag_part == "-") imag = -1;
            else imag = std::stoi(imag_part);

            return GaussianInteger(real, imag);
        }

    private:
        int imag, real;
};

template <typename T>
class Fractie{
    public:
        Fractie(const T& numarator,const T& numitor): numarator(numarator),numitor(numitor){}

        ~Fractie(){}

        Fractie<T>(const T& numarator): numarator(numarator),numitor(T(1)){}

        // Copy constructor
        Fractie(const Fractie<T>& other): numarator(other.numarator), numitor(other.numitor){}
       
        // Move constructor
        Fractie(Fractie<T>&& other): numarator(other.numarator), numitor(other.numitor){}

        // Copy assignment
        void operator=(const Fractie<T>& other){
            numarator = other.numarator;
            numarator = other.numarator;
        }

        // Move assignment
        Fractie<T> operator=(Fractie<T>&& other){
            numarator = other.numarator;
            numarator = other.numarator;
        }

        // Equality operator
        Fractie<T> operator==(const Fractie<T>& other){
            return numarator == other.numarator && numitor == other.numitor;
        }

        Fractie<T> operator+(const Fractie<T>& other){
            return Fractie(
                numarator + other.numarator,
                numitor + other.numitor
            );
        }

        Fractie<T> operator-(const Fractie<T>& other){
            return Fractie(
                numarator - other.numarator,
                numitor - other.numitor
            );
        }

        // Unary Minus
        Fractie<T> operator-(){
            return Fractie(-numarator,numitor);
        }

        Fractie<T> operator*(const Fractie<T>& other){
            return Fractie(
                numarator * other.numarator,
                numitor * other.numitor
            );
        }

        Fractie<T> operator/(const Fractie<T>& other){
            if (numitor == T(0) || other.numitor == T(0)) throw std::overflow_error("Cannot divide by 0");

            return Fractie(
                numarator - other.numarator, 
                numitor - other.numitor
             );
        }

        friend std::ostream& operator<<(std::ostream& os, const Fractie& f) {
            os << f.numarator << "/" << f.numitor;
            return os;
        }

        friend std::istream& operator>>(std::istream& is,  Fractie& f) {
            T numarator, numitor;
            char slash;

            // citim forma: numarator / numitor
            if (is >> numarator >> slash >> numitor) {
                if (slash != '/') {
                    is.setstate(std::ios::failbit);
                } else {
                    f.numarator = numarator;
                    f.numitor = numitor;
                }
            }
            return is;
        }

    private:
        T numarator;
        T numitor;    
};


int main(){
    Fractie<int> a(1,2);
    Fractie<int> b(0,0);

    Fractie<GaussianInteger> c(GaussianInteger(1,1),GaussianInteger(-1,-1));
    Fractie<GaussianInteger> d(GaussianInteger(0),GaussianInteger(0,-1));

    std::cout << a << "\n"
    << b << "\n"
    << c << "\n"
    << d << "\n";

   

    try{
        a/b;
    }catch(std::overflow_error& e){
        std::cout << e.what();
    }
}