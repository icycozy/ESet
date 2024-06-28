#ifndef Treap_Set_HPP
#define Treap_Set_HPP

#include <stdexcept>
#include <cstdlib>
#include <utility>
#include <ctime>

template<class Key, class Compare = std::less<Key>>
class Treap_Set {
    struct Node {
        Key data;
        int priority;  // Priority for heap property
        Node *left, *right, *parent;

        Node(Key data) : data(data), priority(std::rand()), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    Node *root, *minNode;
    Compare comp;
    size_t size_;

    Node* Merge(Node* left, Node* right) {
        ++cntM;
        if (!left || !right) return left ? left : right;
        if (left->priority > right->priority) {
            left->right = Merge(left->right, right);
            if (left->right) left->right->parent = left;
            return left;
        } else {
            right->left = Merge(left, right->left);
            if (right->left) right->left->parent = right;
            return right;
        }
    }

    void Split1(Node* node, const Key& key, Node*& left, Node*& right) {
        ++cntS;
        if (!node) {
            left = right = nullptr;
            return;
        }
        if (comp(node->data, key)) { // node->data < key
            left = node;
            Split1(node->right, key, node->right, right);
            if(node->right) node->right->parent = node;
        } else {
            right = node;
            Split1(node->left, key, left, node->left);
            if(node->left) node->left->parent = node;
        }
        node->parent = nullptr;
    }
    void Split2(Node* node, const Key& key, Node*& left, Node*& right) {
        ++cntS;
        if (!node) {
            left = right = nullptr;
            return;
        }
        if (!comp(key, node->data)) { // node->data <= key
            left = node;
            Split2(node->right, key, node->right, right);
            if(node->right) node->right->parent = node;
        } else {
            right = node;
            Split2(node->left, key, left, node->left);
            if(node->left) node->left->parent = node;
        }
        node->parent = nullptr;
    }

public:

    long long cntM, cntS;
    
    class iterator {
        Node *node;
        const Treap_Set<Key, Compare>* container;
    public:
        iterator() : node(nullptr), container(nullptr) {}
        iterator(const Treap_Set<Key, Compare>* cont, Node* ptr = nullptr) : container(cont), node(ptr) {}
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
            } else {
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
            } else if(node->left) {
                node = node->left;
                while(node->right)
                    node = node->right;
            } else {
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

    Treap_Set() : root(nullptr), minNode(nullptr), size_(0) {
        std::srand(std::time(0));  // Initialize random seed
    }

    Treap_Set(const Treap_Set& other) : root(nullptr), minNode(nullptr), comp(other.comp), size_(other.size_) {
        if (other.root != nullptr) {
            root = copy(other.root, nullptr);
            minNode = root;
            while (minNode && minNode->left != nullptr)
                minNode = minNode->left;
        }
    }

    Node* copy(Node* node, Node* parent) {
        if(!node) return nullptr;
        Node *newNode = new Node(node->data);
        newNode->priority = node->priority;
        newNode->parent = parent;
        newNode->left = copy(node->left, newNode);
        newNode->right = copy(node->right, newNode);
        return newNode;
    }

    Treap_Set& operator = (const Treap_Set& other) {
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

    Treap_Set(Treap_Set&& other) noexcept : root(other.root), minNode(other.minNode), comp(std::move(other.comp)), size_(other.size_) {
        other.root = nullptr;
        other.minNode = nullptr;
        other.size_ = 0;
    }

    Treap_Set& operator = (Treap_Set&& other) noexcept {
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

    ~Treap_Set() {
        clear(root);
        root = nullptr;
        minNode = nullptr;
        size_ = 0;
    }

    void clear(Node *node) {
        if(node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        Key key(std::forward<Args>(args)...);
        auto it = find(key);
        if (it != end()) return {it, false};
        Node *left = nullptr, *right = nullptr;
        Split1(root, key, left, right);
        Node *newNode = new Node(key);
        root = Merge(Merge(left, newNode), right);
        ++size_;
        if (!minNode || comp(key, minNode->data)) minNode = newNode;
        return {iterator(this, newNode), true};
    }

    size_t erase(const Key& key) {
        Node *left = nullptr, *right = nullptr;
        Split1(root, key, left, right);
        Node *midLeft = nullptr, *midRight = nullptr;
        Split2(right, key, midLeft, midRight);
        if (midLeft) {
            Node *tmp = midLeft;
            root = Merge(left, midRight);
            if (minNode == tmp) {
                minNode = root;
                while (minNode && minNode->left != nullptr)
                    minNode = minNode->left;
            }
            delete midLeft;
            --size_;
            return 1;
        } else {
            root = Merge(left, right);
            return 0;
        }
    }

    iterator begin() const noexcept {
        return iterator(this, minNode);
    }

    iterator end() const noexcept {
        return iterator(this, nullptr);
    }

    iterator find(const Key& key) const {
        Node* current = root;
        while (current) {
            if (comp(key, current->data)) {
                current = current->left;
            } else if (comp(current->data, key)) {
                current = current->right;
            } else {
                return iterator(this, current);
            }
        }
        return end();
    }

    iterator lower_bound(const Key& key) const {
        Node *result = nullptr, *p = root;
        while(p) {
            if(!comp(p->data, key)) {
                result = p;
                p = p->left;
            } else p = p->right;
        }
        return iterator(this, result);
    }

    iterator upper_bound(const Key& key) const {
        Node *result = nullptr, *p = root;
        while(p) {
            if(comp(key, p->data)) {
                result = p;
                p = p->left;
            } else p = p->right;
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
