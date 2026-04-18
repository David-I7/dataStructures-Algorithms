#include <cstddef>
#include <iostream>
#include <ostream>
#include <utility>
#include <vector>

template <typename T>
class BinarySearchTree{
    public:
        enum TRASVERSAL_STRATEGY {
            IN_ORDER,
            PRE_ORDER,
            POST_ORDER,
            BFS
        };

        BinarySearchTree(): root(nullptr),_size(0),traversal_strategy(TRASVERSAL_STRATEGY::PRE_ORDER){}

        ~BinarySearchTree(){
            
        }
        
        void insert(const T& data){
            _insert(data);
        }
        
        void insert(T&& data){
            _insert(std::move(data));
        }

        void remove(const T& data){
            if (_size == 0) return;

            Node* cur = root;
            Node* parent = nullptr;
            
            while(cur != nullptr){
                if(data < cur->data){
                    parent = cur;
                    cur = cur->left;
                }else if(data > cur->data){
                    parent = cur;
                    cur = cur->right;
                }else if(cur->data == data) break;
            }

            // data is not present in the tree
            if (!cur) return;

            if(cur->left && cur->right){
                Node* predecessor = cur->left;
                Node* predecessorParent = cur;

                while(predecessor->right != nullptr){
                    predecessorParent = predecessor;
                    predecessor = predecessor->right;
                }

                cur->data = std::move(predecessor->data);
                predecessorParent->right = nullptr;
                delete predecessor;
                --_size;
                return;
            }

            Node* child = cur->left ? cur->left : cur->right;
            cur->left = nullptr;cur->right = nullptr;

            if (!parent){
                root = nullptr;
                delete cur;
            }else if (parent->left == cur){
                parent->left = child;
            }else{
                parent->right = child;
            }

            delete cur;
            --_size;
        }

        bool contains(const T& data) const{
            if(_size == 0) return false;

            Node* cur = root;
            while(cur != nullptr){
                if(data == cur->data) return true;
                else if(data < cur->data){
                    cur = cur->left;
                }else{
                    cur= cur->right;
                }
            }

            return false;
        }

        std::size_t size() const{
            return _size;
        }

        void setTraversalStrategy(TRASVERSAL_STRATEGY strategy){
            this->traversal_strategy = strategy;
        }

        friend std::ostream& operator<<(std::ostream& os,const BinarySearchTree<T>& bst){
            if (bst._size == 0) {
                os << "{}";
                return os;
            } 

            std::vector<Node*> stack;
            

            switch(bst.traversal_strategy){
                case BinarySearchTree<T>::TRASVERSAL_STRATEGY::IN_ORDER:{
                    // stack.push_back(bst.root);

                    // while(!stack.empty()){
                    //     Node* cur = stack.back();

                    //     if (cur->left){
                    //         stack.push_back(cur->left);
                    //     }else{
                    //         std::cout << cur->data << " ";
                    //         stack.pop_back();
                            
                    //         if (cur->right){
                    //             stack.push_back(cur->right);
                    //         }
                    //     }
                    // }

                    // break;
                }
                
                case BinarySearchTree<T>::TRASVERSAL_STRATEGY::PRE_ORDER:{
                    Node* cur = bst.root;

                    while(!stack.empty() || cur){

                        while(cur){
                            std::cout << cur->data << " ";
                            if (cur->right) stack.push_back(cur->right);
                            cur=cur->left;
                        }

                        if (!stack.empty()){
                            cur = stack.back();
                            stack.pop_back();
                        }   
                    }

                    break;
                }

                case BinarySearchTree<T>::TRASVERSAL_STRATEGY::POST_ORDER:{
                    
                }
                case BinarySearchTree<T>::TRASVERSAL_STRATEGY::BFS:{
                    
                }
            }

            return os;
        } 

    private:
        class Node{
            public:
                Node(const T& data,Node* left = nullptr,Node* right = nullptr): data(data),left(left),right(right){}
                
                Node(T&& data,Node* left = nullptr,Node* right = nullptr): data(std::move(data)),left(left),right(right){}

                ~Node() = default;

                T data;
                Node* left;
                Node* right;
        };

        Node* root;
        std::size_t _size;
        TRASVERSAL_STRATEGY traversal_strategy;

        template<typename U>
        void _insert(U&& data){
            if (_size == 0){
                root = new Node(std::forward<U>(data));
                ++_size;
                return;
            }

            Node* cur = root;
            while (cur != nullptr) {
                // duplicates are not stored
                if (data == cur->data) return;
                else if (data < cur->data){
                    if (cur->left){
                        cur = cur->left;
                    }else{
                        cur->left = new Node(std::forward<U>(data));
                        ++_size;
                    }
                }else{
                    if (cur->right){
                        cur = cur->right;
                    }else{
                        cur->right = new Node(std::forward<U>(data));
                        ++_size;
                    }
                }
            }
        }
};


int main(){
    BinarySearchTree<int> bst;

    bst.insert(10);
    bst.insert(5);
    bst.insert(12);
    bst.insert(7);
    bst.insert(6);
    bst.insert(11);
    bst.insert(2);

    std::cout <<bst;
}