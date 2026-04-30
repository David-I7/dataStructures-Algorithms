#include <fstream>
#include <iostream>
#include <string>

struct Node{
    int data;
    Node* next;
};

int randInt(int start, int end){
    return rand() % (end - start + 1) + start;
}

int main(){
    std::ifstream fis("src/data/ruleta.txt");

    if(!fis.is_open()){
        std::cout << "Fiserul nu s-a putut deschide!\n";
        return 1;
    }

    // Numar sectoare
    int k;
    // Numarul maxim de incercari
    int nr;
    // Numarul maxim de sectoare peste care se poate trece la o rotire a ruletei
    int n;

    srand(time(0));

    fis >> k >> nr >> n;

    Node* head = new Node();
    Node* tail = head;
    
    while(fis >> tail->data){
        tail->next = new Node();
        tail = tail->next;
    }

    delete tail->next;
    tail->next = head;

    int score = 0;
    // Numar maxim de incercari
    int s = randInt(1, nr);
    // Pas
    int i=1;

    std::string decizie;
    Node* cur = head;

    while(true){
        if (i > s) break;

        std::cout << "Scor: " << score << "\n";
        std::cout << "Incercari ramase: " << s - i + 1 << "\n";
        std::cout << "Doriti sa rotiti ruleta? (da/nu)\n";

        while(true){
            decizie.clear();
            std::cin >> decizie;

            if(decizie == "da" || decizie == "nu"){
                break;
            }else{
                std::cout << "Va rugam sa tastati 'da' sau 'nu'\n";
            }
        }

        std::cout << "\n";
        if(decizie == "nu") break;

        int r = randInt(1, n);

        while(r > 0){
            cur = cur->next;
            r--;
        }

        score += cur->data;
        i++;
    }

    std::cout << "Scor final: " << score << "\n";

}