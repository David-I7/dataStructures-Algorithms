#include "include/dataStrucutres/iterable/collection/list/arrayList/ArrayList.h"

#include <iostream>
#include <ostream>
#include <vector>
#include "include/dataStrucutres/Object.h"

class Point{
   

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
    {os << "(" << p.x << ", " << p.y << ")\n"; }
    return os;
}


int main()
{
    // ArrayList<Point> arr;

    // arr.add(Point());
    // arr.add(Point());

    // arr.forEach([](const Point& p) {std::cout << "(" << p.x << ", " << p.y << ")\n"; });

    Object a;
    Object b;
    Object &c = b;

    std::cout << a << b;
    std::cout << a.hashCode() << "\n" << b.hashCode() << "\n";
    std::cout << b.equals(c) << "\n";
    std::cout << c.equals(a) << "\n";

    ArrayList<int> arr1;
    ArrayList<int> arr2;

    std::cout << arr1 << arr2 ;
    std::cout << arr1.equals(arr2);

    // std::vector<Point> arr(1);

    // Point a = Point();
    // Point b = Point();

    // arr.push_back(a);
    // arr.push_back(b);

    // for(int i = 0 ; i < arr.size(); ++i){
    //     std::cout << arr[i];
    // }

    return 0;
}