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

        Node(Key data) : data(data), priority(std::rand()), left(nullptr), right(nullptr), ref_count(1) {}
        Node(Key data, int priority, Node* left, Node* right)
            : data(data), priority(priority), left(left), right(right), ref_count(1) {
                if(left) ++left->ref_count;
                if(right) ++right->ref_count;
            }
    };

    using NodePtr = Node*;

    NodePtr root;
    Key min;
    size_t size_;

    NodePtr Merge(NodePtr left, NodePtr right) const {
        if (!left || !right) return left ? left : right;
        if (left->priority > right->priority) {
            left->right = Merge(left->right, right);
            return left;
        } else {
            right->left = Merge(left, right->left);
            return right;
        }
    }

    void Split1(NodePtr node, const Key& key, NodePtr& left, NodePtr& right) const {
        if (!node) {
            left = right = nullptr;
            return;
        }
        if (node->data < key) {
            left = new Node(node->data, node->priority, node->left, nullptr);
            Split1(node->right, key, left->right, right);
        } else {
            right = new Node(node->data, node->priority, nullptr, node->right);
            Split1(node->left, key, left, right->left);
        }
    }

    void Split2(NodePtr node, const Key& key, NodePtr& left, NodePtr& right) const {
        if (!node) {
            left = right = nullptr;
            return;
        }
        if (!(key < node->data)) {
            left = new Node(node->data, node->priority, node->left, nullptr);
            Split2(node->right, key, left->right, right);
        } else {
            right = new Node(node->data, node->priority, nullptr, node->right);
            Split2(node->left, key, left, right->left);
        }
    }

    NodePtr minNode() const {
        NodePtr cur = root;
        while(cur && cur->left) cur = cur->left;
        return cur;
    }

public:
    class iterator { 
        const ESet<Key>* container;
    public:
        bool isend;
        Key kata;
    public:
        
        iterator() : isend(1), container(nullptr) {}
        iterator(const ESet<Key>* cont, NodePtr ptr) : container(cont), isend(1){
            if(ptr) {
                isend = 0;
                kata = ptr->data;
            }
            else isend = 1;
        }
        iterator(const iterator& other) : container(other.container), kata(other.kata), isend(other.isend) {}
        ~iterator() {}

        iterator& operator=(const iterator& other) {
            if (this != &other) {
                isend = other.isend;
                kata = other.kata;
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
            if (isend) return *this;
            Key key = kata;
            NodePtr current = container->root;
            NodePtr next = nullptr;
            while (current) {
                if (key < current->data) {
                    next = current;
                    current = current->left;
                } else {
                    current = current->right;
                }
            }
            if(next) kata = next->data, isend = 0;
            else isend = 1;
            return *this;
        }
        iterator operator -- (int) {
            iterator temp = *this;
            --(*this);
            return temp;
        }
        iterator& operator -- () {
            if (isend) {
                NodePtr node = container->root;
                while (node && node->right) {
                    node = node->right;
                }
                if(node) {
                    isend = 0;
                    kata = node->data;
                }
                else isend = 1;
                return *this;
            }
            Key key = kata;
            NodePtr current = container->root;
            NodePtr prev = nullptr;
            while (current) {
                if (current->data < key) {
                    prev = current;
                    current = current->right;
                } else {
                    current = current->left;
                }
            }
            if (!prev) {
                prev = container->root;
                while(prev->left) prev = prev->left;
            }
            if(prev) kata = prev->data, isend = 0;
            else isend = 1;
            return *this;
        }
        const Key& operator * () const {
            if (isend) throw std::invalid_argument("");
            return kata;
        }
        bool operator == (const iterator& rhs) const {
            return container == rhs.container && ((isend && rhs.isend) || (!isend && !rhs.isend && kata == rhs.kata));
        }
        bool operator != (const iterator& rhs) const {
            return !(*this == rhs);
        }
    };

    ESet() : root(nullptr), size_(0) {
        std::srand(std::time(0));  // Initialize random seed
    }

    ESet(const ESet& other) : root(other.root), size_(other.size_) {
        if (root) {
            ++root->ref_count;
        }
    }

    ESet& operator = (const ESet& other) {
        if (this != &other) {
            clear(root);
            root = other.root;
            if (root) {
                ++root->ref_count;
            }
            size_ = other.size_;
        }
        return *this;
    }

    ESet(ESet&& other) noexcept : root(other.root), size_(other.size_) {
        other.root = nullptr;
        other.size_ = 0;
    }

    ESet& operator = (ESet&& other) noexcept {
        if (this != &other) {
            clear(root);
            root = other.root;
            size_ = other.size_;
            other.root = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~ESet() {
        clear(root);
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        Key key(std::forward<Args>(args)...);
        auto it = find(key);
        if (it != end()) return {it, false};
        NodePtr left = nullptr, right = nullptr;
        Split1(root, key, left, right);
        NodePtr newNode = new Node(key);
        clear(root);
        root = Merge(Merge(left, newNode), right);
        ++size_;
        return {iterator(this, newNode), true};
    }

    size_t erase(const Key& key) {
        NodePtr left = nullptr, right = nullptr;
        Split1(root, key, left, right);
        clear(root);
        NodePtr midLeft = nullptr, midRight = nullptr;
        Split2(right, key, midLeft, midRight);
        clear(right);
        root = Merge(left, midRight); 
        if (midLeft) {
            --size_;
            clear(midLeft);           
            return 1;
        } else {
            return 0;
        }
    }

    iterator begin() const noexcept{
        return iterator(this, minNode());
    }
    iterator end() const noexcept {
        return iterator(this, nullptr);
    }

    iterator find(const Key& key) const {
        NodePtr current = root;
        while (current) {
            if (key < current->data) {
                current = current->left;
            } else if (current->data < key) {
                current = current->right;
            } else {
                return iterator(this, current);
            }
        }
        return end();
    }

    size_t range(const Key& l, const Key& r) const {
        if (r < l) return 0;
        return countRange(root, l, r);
    }

    size_t countRange(NodePtr p, const Key& l, const Key& r) const {
        if (!p) return 0;
        size_t res = 0;
        if (!(p->data < l) && !(r < p->data)) ++res;
        if (!(r < p->data)) res += countRange(p->right, l, r);
        if (!(p->data < l)) res += countRange(p->left, l, r);
        return res;
    }

private:
    void clear(NodePtr node) {
        if (node) {
            if (--node->ref_count == 0) {
                clear(node->left);
                clear(node->right);
                node->left = node->right = nullptr;
                delete node;
            }
        }
    }
};

const int N = 100000;
inline ll read(){
	ll x=0;bool f=1;char c;
	while((c=getchar())<'0'||c>'9') if(c=='-') f=0;
	while(c>='0'&&c<='9') x=(x<<3)+(x<<1)+(c^48),c=getchar();
	return f?x:-x;
}
inline void write(size_t x) {
    static int sta[35];
    int top = 0;
    do {
        sta[top++] = x % 10, x /= 10;
    } while (x);
    while (top) putchar(sta[--top] + 48);
    putchar('\n');
}

int main() {
    ESet<long long> s[N];
    ESet<long long>::iterator it;
    long long it_a;
    int op, lst=0, valid = 0;
    while (scanf("%d", &op) != EOF) {
        long long a, b, c;
        switch (op) {
            case 0: {
                a = read();
                b = read();
                auto p = s[a].emplace(b);
                if (p.second) {
                    it = p.first;
                    it_a = a;
                    valid = 1;
                }
                break;
            }
            case 1:
                a = read();
                b = read();
                if (valid && it_a == a && *it == b) valid = 0;
                s[a].erase(b);
                break;
            case 2:
                a = read();
                s[++lst] = s[a];
                break;
            case 3: {
                a = read();
                b = read();
                auto it2 = s[a].find(b);
                if (it2 != s[a].end()) {
                    printf("true\n");
                    it = it2;
                    it_a = a;
                    valid = 1;
                } else {
                    printf("false\n");
                }
                break;
            }
            case 4:
                a = read();
                b = read();
                c = read();
                write(s[a].range(b, c));
                break;
            case 5:
                if (valid) {
                    if (it == s[it_a].begin())
                        valid = 0;
                }
                if (valid) {
                    write(*(--it));
                } 
                else {
                    printf("-1\n");
                }
                break;
            case 6:
                if (valid) {
                    ++it;
                    if (it.isend) valid = 0;
                    else write(*it);
                }
                if (!valid) {
                    printf("-1\n");
                }
                break;
        }
    }
    return 0;
}
