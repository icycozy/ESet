#ifndef AVL_Set_HPP
#define AVL_Set_HPP

#include <stdexcept>
#include <utility>

template<class Key, class Compare = std::less<Key>>
class AVL_Set {
    struct Node {
        Key data;
        int height;
        Node *left, *right, *parent;

        Node(Key data) : data(data), height(1), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    Node *root, *minNode;
    Compare comp;
    size_t size_;

    int height(Node *node) {
        return node ? node->height : 0;
    }
    int balanceFactor(Node *node) {
        return node ? height(node->left) - height(node->right) : 0;
    }
    void updateHeight(Node *node) {
        node->height = 1 + (height(node->left)>height(node->right)?height(node->left):height(node->right));
    }

    void rotateLeft(Node *node) {
        Node *rightChild = node->right;
        node->right = rightChild->left;
        if (rightChild->left != nullptr) rightChild->left->parent = node;
        rightChild->parent = node->parent;
        if (node->parent == nullptr) root = rightChild;
        else if (node == node->parent->left) node->parent->left = rightChild;
        else node->parent->right = rightChild;
        rightChild->left = node;
        node->parent = rightChild;
        updateHeight(node);
        updateHeight(rightChild);
    }

    void rotateRight(Node *node) {
        Node *leftChild = node->left;
        node->left = leftChild->right;
        if (leftChild->right != nullptr) leftChild->right->parent = node;
        leftChild->parent = node->parent;
        if (node->parent == nullptr) root = leftChild;
        else if (node == node->parent->left) node->parent->left = leftChild;
        else node->parent->right = leftChild;
        leftChild->right = node;
        node->parent = leftChild;
        updateHeight(node);
        updateHeight(leftChild);
    }

    void balance(Node *&node) {
        Node *parent = node->parent;
        updateHeight(node);
        int bf = balanceFactor(node);
        if (bf > 1) {
            if (balanceFactor(node->left) < 0) {
                rotateLeft(node->left);
            }
            rotateRight(node);
        } else if (bf < -1) {
            if (balanceFactor(node->right) > 0) {
                rotateRight(node->right);
            }
            rotateLeft(node);
        }
        if(parent) balance(parent);
    }

    void clear(Node *node) {
        if(node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    Node* copy(Node* node, Node* parent) {
        if(!node) return nullptr;
        Node *newNode = new Node(node->data);
        newNode->height = node->height;
        newNode->parent = parent;
        newNode->left = copy(node->left, newNode);
        newNode->right = copy(node->right, newNode);
        return newNode;
    }

public:
    class iterator {
        Node *node;
        const AVL_Set<Key, Compare>* container;
    public:
        iterator() : node(nullptr), container(nullptr) {}
        iterator(const AVL_Set<Key, Compare>* cont, Node* ptr = nullptr) : container(cont), node(ptr) {}
        iterator(const iterator& other) : node(other.node), container(other.container) {}

        iterator& operator=(const iterator& other) {
            if (this != &other) {
                node = other.node;
                container = other.container;
            }
            return *this;
        }

        iterator operator ++ (int) {          
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        iterator& operator ++ () {
            if(node == nullptr) return *this;
            if(node->right != nullptr) {
                node = node->right;
                while(node->left != nullptr)
                    node  = node->left;
            }
            else {
                Node *p = node->parent;
                while(p != nullptr && node == p->right) {
                    node = p;
                    p = p->parent;
                }
                node = p;
            }
            return *this;
        }

        iterator operator -- (int) {
            iterator temp = *this;
            --(*this);
            return temp;
        }

        iterator& operator -- () {
            if(*this == container->begin()) return *this;
            if(node == nullptr) {
                node = container->root;
                while(node && node->right)
                    node = node->right;
            }
            else if(node->left) {
                node = node->left;
                while(node->right)
                    node = node->right;
            }
            else {
                Node *p = node->parent;
                while(p && node == p->left) {
                    node = p;
                    p = p->parent;
                }
                node = p;
            }
            return *this;
        }

        const Key& operator * () const {
            if(node == nullptr) throw std::invalid_argument("");
            return node->data;
        }

        bool operator == (const iterator& rhs) const {
            return node == rhs.node && container == rhs.container;
        }

        bool operator != (const iterator& rhs) const {
            return !(*this == rhs);
        }
    };

    AVL_Set() : root(nullptr), minNode(nullptr), size_(0) {}

    AVL_Set(const AVL_Set& other) : root(nullptr), minNode(nullptr), comp(other.comp), size_(other.size_) {
        if (other.root != nullptr) {
            root = copy(other.root, nullptr);
            minNode = root;
            while (minNode && minNode->left != nullptr)
                minNode = minNode->left;
        }
    }

    AVL_Set& operator = (const AVL_Set& other) {
        if(this != &other) {
            clear(root);
            root = nullptr;
            root = copy(other.root, nullptr);
            comp = other.comp;
            size_ = other.size_;
            minNode = root;
            while (minNode && minNode->left != nullptr)
                minNode = minNode->left;
        }
        return *this;
    }

    AVL_Set(AVL_Set&& other) noexcept : root(other.root), minNode(other.minNode), comp(std::move(other.comp)), size_(other.size_) {
        other.root = nullptr;
        other.minNode = nullptr;
        other.size_ = 0;
    }

    AVL_Set& operator = (AVL_Set&& other) noexcept {
        if (this != &other) {
            clear(root);
            root = other.root;
            minNode = other.minNode;
            size_ = other.size_;
            comp = std::move(other.comp);
            other.root = nullptr;
            other.minNode = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~AVL_Set() {
        clear(root);
        root = nullptr;
        minNode = nullptr;
        size_ = 0;
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        Node *newNode = new Node(Key(std::forward<Args>(args)...));
        if(!root) {
            root = newNode;
            minNode = newNode;
            size_ = 1;
            return {iterator(this, root), true};
        }
        Node *cur = root, *parent = nullptr;
        while(cur) {
            parent = cur;
            if(comp(newNode->data, cur->data))
                cur = cur->left;
            else if(comp(cur->data, newNode->data))
                cur = cur->right;
            else {
                delete newNode;
                return {iterator(this, cur), false};
            }
        }
        newNode->parent = parent;
        if(comp(newNode->data, parent->data)) {
            parent->left = newNode;
            if(parent == minNode) minNode = newNode;
        }
        else parent->right = newNode;

        balance(newNode);
        ++size_;
        return {iterator(this, newNode), true};
    } 

    size_t erase(const Key& key) {
        Node *node = root, *child = nullptr;
        while(node) {
            if(comp(key, node->data))
                node = node->left;
            else if(comp(node->data, key)) node = node->right;
            else break;
        }
        if(!node) return 0;

        if (node == minNode) {
            if (node->right) {
                minNode = node->right;
                while (minNode->left != nullptr)
                    minNode = minNode->left;
            }
            else minNode = node->parent;
        }

        if (node->left != nullptr && node->right != nullptr) {
            Node *successor = node->right, *parent = node->parent;
            while (successor->left != nullptr) {
                successor = successor->left;
            }

            child = successor->right;
            if(child) child->parent=node;
            if(!parent) root = successor;
            else if(node==parent->left) parent->left=successor;
            else parent->right=successor; 

            if (successor->parent != node) {
                node->parent = successor->parent;
                successor->parent->left=node;
                successor->parent=parent;
                node->left->parent=node->right->parent=successor;
                successor->left=node->left;
                successor->right=node->right;
                node->right=child;
                node->left=nullptr;
            }
            else {
                node->parent=successor;
                successor->left=node->left;
                successor->right=node;
                node->left->parent=successor;
                node->left=nullptr;
                node->right=child;
                successor->parent=parent;
            }       
        }
        else {
            if (node->left != nullptr) child = node->left;
            else child = node->right;   
        }         

        if(child != nullptr)
            child->parent = node->parent;
        if (node->parent == nullptr) root = child;
        else if (node == node->parent->left) node->parent->left = child;
        else node->parent->right = child;

        if(node->parent) balance(node->parent);
        delete node;
        --size_;
        return 1;
    }

    iterator begin() const noexcept {
        return iterator(this, minNode);
    }

    iterator end() const noexcept {
        return iterator(this, nullptr);
    }

    iterator find(const Key& key) const {
        Node *p = root;
        while(p != nullptr) {
            if(comp(key, p->data)) p = p->left;
            else if(comp(p->data, key)) p = p->right;
            else return iterator(this, p);
        }
        return end();
    }

    iterator lower_bound(const Key& key) const {
        Node *result = nullptr, *p = root;
        while(p) {
            if(!comp(p->data, key)) {
                result = p;
                p = p->left;
            }
            else p = p->right;
        }
        return iterator(this, result);
    }

    iterator upper_bound(const Key& key) const {
        Node *result = nullptr, *p = root;
        while(p) {
            if(comp(key, p->data)) {
                result = p;
                p = p->left;
            }
            else p = p->right;
        }
        return iterator(this, result);
    }

    size_t range(const Key& l, const Key& r) const {
        if(comp(r, l)) return 0;
        return countRange(root, l, r);
    }

    size_t countRange(Node *p, const Key& l, const Key& r) const {
        if(!p) return 0;
        size_t res = 0;
        if(!comp(p->data, l) && !comp(r, p->data)) ++res;
        if(!comp(r, p->data)) res += countRange(p->right, l, r);
        if(!comp(p->data, l)) res += countRange(p->left, l, r);
        return res;
    }

    size_t size() const noexcept {
        return size_;
    }
};

#endif
