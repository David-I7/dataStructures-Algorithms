#include <iostream>
#include <fstream>

void preorderTraversal(int n, int* nodeInfo,int* leftChild,int* rightChild){
    int stack[n];
    
    int current = 0;
    int size = 0;
    while(current != -1 || size > 0){
        
        while(current != -1){
            std::cout << nodeInfo[current] << " ";
            if(rightChild[current] != -1) stack[size++] = rightChild[current];
            current = leftChild[current];
        }

        if(size > 0){
            current = stack[--size];
        }
    }
    
}

void inorderTraversal(int n, int* nodeInfo,int* leftChild,int* rightChild){
    int stack[n];
    
    int current = 0;
    int size = 0;
    while(current != -1 || size > 0){
        
        while(current != -1){
            stack[size++] = current;
            current = leftChild[current];
        }

        if(size > 0){
            current = stack[--size];
            std::cout << nodeInfo[current] << " ";
            current = rightChild[current];
        }
    }

}

void postorderTraversal(int n, int* nodeInfo,int* leftChild,int* rightChild){
    int stack[n];
    
    int current = 0;
    int prev = -1;
    int size = 0;
    while(current != -1 || size > 0){
        
        if(current != -1){
            stack[size++] = current;
            current = leftChild[current];
        }else{
            int stackTop = stack[size - 1];

            if(rightChild[stackTop] != -1 && prev != rightChild[stackTop]){
                current = rightChild[stackTop];
            }else{
                prev = stackTop;
                std::cout << nodeInfo[prev] << " ";
                size--;
            }
        }
    }
}

int main() {
    std::ifstream fis("src/data/arborebinar.txt");

    if(!fis.is_open()){
        std::cout << "Fiserul nu s-a putut deschide!\n";
        return 1;
    }

    int n = 0;
    
    fis >> n;
    
    int nodeInfo[n];
    int leftChild[n];
    int rightChild[n];

    for (int i = 0; i < n; i++)
        fis >> nodeInfo[i];

    for (int i = 0; i < n; i++)
        fis >> leftChild[i];

    for (int i = 0; i < n; i++)
        fis >> rightChild[i];

    std::cout << "Info: ";
    for (int i = 0; i < n; i++)
        std::cout << nodeInfo[i] << " ";
    std::cout << "\n";

    std::cout << "Stanga: ";
    for (int i = 0; i < n; i++)
        std::cout << leftChild[i] << " ";
    std::cout << "\n";

    std::cout << "Dreapta: ";
    for (int i = 0; i < n; i++)
        std::cout << rightChild[i] << " ";
    std::cout << "\n";

    std::cout << "\nParcurgerea in preordine:\n";
    preorderTraversal(n,nodeInfo,leftChild,rightChild);

    std::cout << "\n\nParcurgerea in inordine:\n";
    inorderTraversal(n,nodeInfo,leftChild,rightChild);
    
    std::cout << "\n\nParcurgerea in postordine:\n";
    postorderTraversal(n,nodeInfo,leftChild,rightChild);

    std::cout << "\n";
    return 0;
}