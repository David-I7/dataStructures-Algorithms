#include <cstddef>
#include <iostream>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string.h>

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
            if (numitor == T(0) || other.numitor == T(0)) throwDivisionByZeroException();

            return Fractie(
                numarator - other.numarator, 
                numitor - other.numitor
             );
        }

        friend std::ostream& operator<<(std::ostream& os, const Fractie& f) {
            os << "(" << f.numarator << "/" << f.numitor << ")";
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

        double toDecimal(){
            return numarator / numitor;
        }

        void throwDivisionByZeroException(){
            throw std::overflow_error("Cannot divide by 0");
        }

};


int main(){
    Fractie<int> a(1,2);
    Fractie<int> b(0,0);

    int c;
    std::cin >> c;

    try{
        a/b;
    }catch(std::overflow_error& e){
        std::cout << e.what();
    }
}