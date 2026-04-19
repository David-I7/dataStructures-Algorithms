#include <iostream>

using namespace std;

class C{};

class A{
    public:

    A(){
        cout << "Creating A \n";
    }

    virtual ~A(){
        cout << "Destroying A \n";
    };

    void get(int* const x){
        *x = 1;
    }

    void get(double* const x){
        *x = 1.0;
    }

    virtual const char* ok() = 0;
};

const char* A::ok(){
    return "OK";
}

class B : public A, private C{
    public: 
    // Expune metodele clasei A in caz ca definim si noi un overload pentru aceasta
    using A::get;

    void get(char* const x){
        *x = 'A';
    }

    B(){
        cout << "Creating B \n";
    }

    ~B(){
        cout << "Destroying B \n";
    };

    const char* ok() override{
        // Pentru a apela obiectul 'super'
        A::ok();
    };
};

int main(){
    {
    // A nu se poate instantia pentru ca toate contine o metoda virtuala = 0 (chiar daca are propria implementare)
    B b2 = B();
    // Trebuie sa facem destructorul virtual in clasa de baza pentru a ne asigura ca se apeleaza destructorul pentru intreaga ierarhie de clase polymorfice 
    A* b= new B();

    
    delete b;
    }
}