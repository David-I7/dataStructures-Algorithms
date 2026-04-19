#include <cstddef>
#include <iostream>
#include <ostream>
#include <utility>
#include <vector>
#include <queue>

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
            freeElements();
        }

        BinarySearchTree(const BinarySearchTree<T>& other){
            copyElements(other);
            traversal_strategy = other.traversal_strategy;
        }

        BinarySearchTree(BinarySearchTree<T>&& other){
            root = other.root;
            _size = other._size;
            traversal_strategy = other.traversal_strategy;

            root = nullptr;
            _size = 0;
            traversal_strategy = BinarySearchTree<T>::PRE_ORDER;
        }
        
        BinarySearchTree<T>& operator=(const BinarySearchTree<T>& other){
             if(&other == this) return *this;

            freeElements();
            copyElements(other);
            traversal_strategy = other.traversal_strategy;
        }

        BinarySearchTree<T>& operator=(BinarySearchTree<T>&& other){
            if(&other == this) return *this;

            freeElements();
            root = other.root;
            _size = other._size;
            traversal_strategy = other.traversal_strategy;

            root = nullptr;
            _size = 0;
            traversal_strategy = BinarySearchTree<T>::PRE_ORDER;
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

            switch(bst.traversal_strategy){
                case BinarySearchTree<T>::TRASVERSAL_STRATEGY::IN_ORDER:{
                    std::vector<Node*> stack;
                    Node* cur = bst.root;

                    while(!stack.empty() || cur){
                        while(cur){
                            stack.push_back(cur);
                            cur = cur->left;
                        }

                        if (!stack.empty()){
                            cur = stack.back();
                            stack.pop_back();
                            std::cout << cur->data << " ";
                            cur = cur->right;
                        }
                    }

                    break;
                }
                
                case BinarySearchTree<T>::TRASVERSAL_STRATEGY::PRE_ORDER:{
                    std::vector<Node*> stack;
                    Node* cur = bst.root;
    
                    while(!stack.empty() || cur){
                        while(cur){
                            std::cout << cur->data << " ";
                            if (cur->right) stack.push_back(cur->right);
                            cur=cur->left;
                        } 

                        if(!stack.empty()){
                            cur = stack.back();
                            stack.pop_back();
                        }
                    }

                    break;
                }

                case BinarySearchTree<T>::TRASVERSAL_STRATEGY::POST_ORDER:{
                    std::vector<Node*> stack;
                    Node* cur = bst.root;
                    Node* lastVisited = nullptr;

                    while(!stack.empty() || cur){
                        if(cur){
                            stack.push_back(cur);
                            cur = cur->left;
                        }else{
                            Node* top = stack.back();

                            if(top->right && lastVisited != top->right){
                                cur = top->right;
                            }else{
                                if(!stack.empty()){
                                    lastVisited = stack.back();
                                    std::cout << lastVisited->data << " ";
                                    stack.pop_back();
                                }
                            }
                        }
                    }

                    break;
                }
                case BinarySearchTree<T>::TRASVERSAL_STRATEGY::BFS:{
                    std::queue<Node*> queue;
                    queue.push(bst.root);

                    while(!queue.empty()){
                        std::size_t size = queue.size();
                        for(std::size_t i = 0; i < size;++i){
                            Node* cur = queue.front();
                            queue.pop();
                            std::cout << cur->data << " ";
                            
                            if(cur->left) queue.push(cur->left);
                            if(cur->right) queue.push(cur->right);
                        }
                    }

                    break;
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

        void freeElements(){
            if (_size == 0) return;

            std::vector<Node*> stack;
            Node* cur = root;
            Node* prev = nullptr;

            while(!stack.empty() || cur){
                while(cur){
                    if(cur->right){
                        stack.push_back(cur->right);
                    }
                    prev = cur;
                    cur = cur->left;
                    prev->left = nullptr;
                    prev->right = nullptr;
                    delete prev;
                }

                if(!stack.empty()){
                    cur = stack.back();
                    stack.pop_back();
                }
            }

            _size = 0;
            root = nullptr;
        }

        void copyElements(const BinarySearchTree<T>& other){
            if(other.root == nullptr){
                root = nullptr;
                _size = 0;
                return;
            }

            std::vector<std::pair<Node*,Node*>> stack;
            Node* cur = other.root;
            Node* copy = new Node(other.root->data);
            std::pair<Node*,Node*> nodePair;
            root = copy;
            _size = other._size;
           
            while(!stack.empty() || cur){
                while(cur){
                    if(cur->right){
                        copy->right = new Node(cur->right->data);
                        stack.push_back({cur->right,copy->right});
                    }
                    if(cur->left){
                        copy->left = new Node(cur->left->data);
                        copy = copy->left;
                        cur = cur->left;
                    }else{
                        cur = nullptr;
                    }
                }

                if (!stack.empty()){
                    nodePair = stack.back();
                    cur = nodePair.first;
                    copy = nodePair.second;
                    stack.pop_back();
                    
                }
            }
        }

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

    std::cout << "PRE ORDER: " << bst << "\n";
    bst.setTraversalStrategy(BinarySearchTree<int>::IN_ORDER);
    std::cout << "IN ORDER: " << bst << "\n";
    bst.setTraversalStrategy(BinarySearchTree<int>::POST_ORDER);
    std::cout << "POST ORDER: " << bst << "\n";
    bst.setTraversalStrategy(BinarySearchTree<int>::BFS);
    std::cout << "BFS: " << bst << "\n";
}