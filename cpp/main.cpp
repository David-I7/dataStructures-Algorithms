#include "java/iterable/collection/list/arrayList/ArrayList.h"

#include <iostream>
#include <ostream>
#include "java/Object.h"

class Point: public Object{
   

    public:
    int x;
    int y;

    Point(int x, int y): x(x), y(y){};
    Point(): x(0), y(0){
        std::cout << "Constructor called\n";
    };
    ~Point(){
        std::cout << "Destructor called\n";
    };

    Point(Point&& other){
        std::cout << "Move Constructor called\n";
        x = other.x;
        y= other.y;
    }
    Point(const Point& other){
        std::cout << "Copy Constructor called\n";
        x = other.x;
        y= other.y;
    }

    void operator=(const Point& other){
        std::cout << "Copy Assignment called\n";
        x = other.x;
        y= other.y;
    }
    void operator=(Point&& other){
        std::cout << "Move Assignment called\n";
        x = other.x;
        y= other.y;
    }

    friend std::ostream& operator<<(std::ostream& os,const Point& p);
};

std::ostream& operator<<(std::ostream& os,const Point& p){
    {os << "(" << p.x << ", " << p.y << ")"; }
    return os;
}


int main()
{
    // arr.add(Point());
    // arr.add(Point());

    // arr.forEach([](const Point& p) {std::cout << "(" << p.x << ", " << p.y << ")\n"; });

    // Object a;
    // Object b;
    // Object &c = b;

    // std::cout << a << b;
    // std::cout << a.hashCode() << "\n" << b.hashCode() << "\n";
    // std::cout << b.equals(c) << "\n";
    // std::cout << c.equals(a) << "\n";

    // ArrayList<int> arr1;
    // ArrayList<int> arr2;

    // std::cout << arr1 << arr2 ;
    // std::cout << arr1.equals(arr2);

    ArrayList<Point> arr;

    // Point a = Point();
    // Point b = Point();

    arr.add(Point());
    arr.add(Point());

    std::cout << arr;

    return 0;
}