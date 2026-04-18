#include <iostream>
#include <vector>
#include <string>

class ResourceMover {
public:
    // 1. Primitive: Just a value copy
    int id = 0;

    // 2. Class: Calls std::string's move constructor (efficient)
    std::string name = "Default";

    // 3. Pointer: COPIES the address (DANGEROUS in default move)
    int* rawPtr = nullptr;

    // Tell the compiler to generate the default move assignment
    ResourceMover& operator=(ResourceMover&& other) = default;
    
    // Constructor for demonstration
    ResourceMover(int i, std::string n, int val) : id(i), name(n) {
        rawPtr = new int(val);
    }
};