#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <utility>
#include <ctime>

typedef long long ll;

template<class Key>
class ESet {
    struct Node {
        Key data;
        int priority;
        Node* left;
        Node* right;
        int ref_count;
        size_t siz;

        Node(Key data) : data(data), priority(std::rand()), left(nullptr), right(nullptr), ref_count(1), siz(1) {}
        Node(Key data, int priority, Node* left, Node* right)
            : data(data), priority(priority), left(left), right(right), ref_count(1) {
                siz = 1;
                if(left) ++left->ref_count, siz+=left->siz;
                if(right) ++right->ref_count, siz+=right->siz;
            }
    };

    using NodePtr = Node*;

    NodePtr root;
    size_t size;

    NodePtr Merge(NodePtr left, NodePtr right) const {
        if (!left || !right) return left ? left : right;
        if (left->priority > right->priority) {
            left->right = Merge(left->right, right);
            left->siz = 1 + (left->left ? left->left->siz : 0) + (left->right ? left->right->siz : 0);
            return left;
        } else {
            right->left = Merge(left, right->left);
            right->siz = 1 + (right->left ? right->left->siz : 0) + (right->right ? right->right->siz : 0);
            return right;
        }
    }

    void Split1(NodePtr node, const Key& key, NodePtr& left, NodePtr& right) const {
        if (!node) {
            left = right = nullptr;
            return;
        }
        if (node->data < key) {
            if (node->ref_count == 0) {
                left = node;
                ++node->ref_count;
                if(node->right) --node->right->ref_count;
            } else {
                left = new Node(node->data, node->priority, node->left, nullptr);
            }
            Split1(node->right, key, left->right, right);
            left->siz = 1 + (left->left ? left->left->siz : 0) + (left->right ? left->right->siz : 0);
        } else {
            if (node->ref_count == 0) {
                right = node;
                ++node->ref_count;
                if(node->left) --node->left->ref_count;
            } else {
                right = new Node(node->data, node->priority, nullptr, node->right);
            }
            Split1(node->left, key, left, right->left);
            right->siz = 1 + (right->left ? right->left->siz : 0) + (right->right ? right->right->siz : 0);
        }
    }

    void Split2(NodePtr node, const Key& key, NodePtr& left, NodePtr& right) const {
        if (!node) {
            left = right = nullptr;
            return;
        }
        if (key >= node->data) {
            if (node->ref_count == 0) {
                left = node;
                ++node->ref_count;
                if(node->right) --node->right->ref_count;
            } else {
                left = new Node(node->data, node->priority, node->left, nullptr);
            }
            Split2(node->right, key, left->right, right);
            left->siz = 1 + (left->left ? left->left->siz : 0) + (left->right ? left->right->siz : 0);
        } else {
            if (node->ref_count == 0) {
                right = node;
                ++node->ref_count;
                if(node->left) --node->left->ref_count;
            } else {
                right = new Node(node->data, node->priority, nullptr, node->right);
            }
            Split2(node->left, key, left, right->left);
            right->siz = 1 + (right->left ? right->left->siz : 0) + (right->right ? right->right->siz : 0);
        }
    }

public:

    Key minK, maxK;

    ESet() : root(nullptr), size(0) {
        std::srand(std::time(0));  // Initialize random seed
    }

    ESet(const ESet& other) : root(other.root), minK(other.minK), maxK(other.maxK), size(other.size) {
        if (root) {
            ++root->ref_count;
        }
    }

    ESet& operator = (const ESet& other) {
        if (this != &other) {
            clear(root);
            root = other.root;
            minK = other.minK;
            maxK = other.maxK;
            size = other.size;
            if (root) {
                ++root->ref_count;
            }
        }
        return *this;
    }

    ESet(ESet&& other) noexcept : root(other.root), size(other.size) {
        minK = other.minK;
        maxK = other.maxK;
        other.root = nullptr;
        other.size = 0;
    }

    ESet& operator = (ESet&& other) noexcept {
        if (this != &other) {
            clear(root);
            root = other.root;
            size = other.size;
            minK = other.minK;
            maxK = other.maxK;
            other.root = nullptr;
            other.size = 0;
        }
        return *this;
    }

    ~ESet() {
        clear(root);
        root = nullptr;
    }

    template <class... Args>
    bool emplace(Args&&... args) {
        Key key(std::forward<Args>(args)...);
        if (find(key)) return false;
        if (empty()) {
            root = new Node(key);
            minK = maxK = key;
            size = 1;
            return true;
        }
        if (key < minK) minK = key;
        if (maxK < key) maxK = key;
        NodePtr left = nullptr, right = nullptr;
        --root->ref_count;
        Split1(root, key, left, right);
        NodePtr newNode = new Node(key);
        root = Merge(Merge(left, newNode), right);
        ++size;
        return true;
    }

    size_t erase(const Key& key) {
        if (!find(key)) return 0;

        NodePtr left = nullptr, right = nullptr;
        --root->ref_count;
        Split1(root, key, left, right);
        NodePtr midLeft = nullptr, midRight = nullptr;
        if(right) --right->ref_count;
        Split2(right, key, midLeft, midRight);
        root = Merge(left, midRight);

        if (midLeft) {
            --size;
            clear(midLeft);
            if (key == minK) updateMin();
            if (key == maxK) updateMax();
            if(!size) root = nullptr;
            return 1;
        } else {
            return 0;
        }
    }

    bool find(const Key& key) const {
        if(empty()) return false;
        NodePtr current = root;
        while (current) {
            if (key < current->data) {
                current = current->left;
            } else if (current->data < key) {
                current = current->right;
            } else {
                return true;
            }
        }
        return false;
    }

    Key Pre(const Key &x) const {
        NodePtr current = root;
        Key predecessor;
        while (current) {
            if (current->data < x) {
                predecessor = current->data;
                current = current->right;
            } else {
                current = current->left;
            }
        }
        return predecessor;
    }

    Key Nxt(const Key &x) const {
        NodePtr current = root;
        Key successor;
        while (current) {
            if (x < current->data) {
                successor = current->data;
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return successor;
    }

    size_t range(const Key& l, const Key& r) {
        if(empty()) return 0;
        NodePtr left = nullptr, right = nullptr, midLeft = nullptr, midRight = nullptr;
        --root->ref_count;
        Split1(root, l, left, right);
        if(right) --right->ref_count;
        Split2(right, r, midLeft, midRight);
        size_t result = midLeft ? midLeft->siz : 0;
        root = Merge(Merge(left, midLeft), midRight);
        return result;
    }

    bool empty() const {
        return size == 0;
    }

private:
    void clear(NodePtr node) {
        if (node && --node->ref_count == 0) {
            clear(node->left);
            clear(node->right);
            delete node;
            node = nullptr;
        }
    }

    void updateMin() {
        if(empty()) return;
        NodePtr current = root;
        minK = current->data;
        while (current->left) {
            current = current->left;
            minK = current->data;
        }
    }

    void updateMax() {
        if(empty()) return;
        NodePtr current = root;
        maxK = current->data;
        while (current->right) {
            current = current->right;
            maxK = current->data;
        }
    }
};