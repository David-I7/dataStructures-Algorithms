#include "dataStrucutres/Object.h"
#include "iostream"

namespace TestUtil {
    class Point: public Object{
   

    public:
    int x;
    int y;

    Point(int x, int y): x(x), y(y){};
    Point(): x(0), y(0){
    };
    ~Point(){
        
    };

    Point(Point&& other){
        
        x = other.x;
        y= other.y;
    }
    Point(const Point& other){
       
        x = other.x;
        y= other.y;
    }

    void operator=(const Point& other){
       
        x = other.x;
        y= other.y;
    }
    void operator=(Point&& other){
      
        x = other.x;
        y= other.y;
    }

    friend std::ostream& operator<<(std::ostream& os,const Point& p);
};
};