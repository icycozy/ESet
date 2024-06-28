#include "/home/xxl/projects/ESet/RBT_copy.hpp"
#include "/home/xxl/projects/ESet/AVL_copy.hpp"
#include "/home/xxl/projects/ESet/Treap_copy.hpp"
#include <set>
#include <iostream>
#define ll long long

inline ll read(){
	ll x=0;bool f=1;char c;
	while((c=getchar())<'0'||c>'9') if(c=='-') f=0;
	while(c>='0'&&c<='9') x=(x<<3)+(x<<1)+(c^48),c=getchar();
	return f?x:-x;
}
inline void write(size_t x) {
    if(x<0) putchar('-'), x=-x;
    static int sta[35];
    int top = 0;
    do {
        sta[top++] = x % 10, x /= 10;
    } while (x);
    while (top) putchar(sta[--top] + 48);
    putchar('\n');
}

int count_elements_in_range(const std::set<ll>& my_set, ll low, ll high) {
    // 找到不小于low的第一个元素的迭代器
    auto it_low = my_set.lower_bound(low);
    // 找到大于high的第一个元素的迭代器
    auto it_high = my_set.upper_bound(high);
    // 使用std::distance计算两个迭代器之间的距离，即区间内的元素数量
    return std::distance(it_low, it_high);
}

int main() {
    freopen("/home/xxl/projects/ESet/test/1.in", "r", stdin);
    srand(0);
    clock_t start, end;
    clock_t alls, alle;
    std::set<long long> s[25];
    std::set<long long>::iterator it;
    long long it_a;
    int op, lst=0, valid = 0;
    int t0=0, t1=0, t2=0, t3=0, t4=0, t5=0, t6=0;
    int cnt0=0, cnt1=0, cnt2=0, cnt3=0, cnt4=0, cnt5=0, cnt6=0;
    alls = clock();
    while (scanf("%d", &op) != EOF) {
        long long a, b, c;
        switch (op) {
            case 0: {   
                ++cnt0;        
                a=read(); b=read();
                start = clock();
                auto p=s[a].emplace(b);
                if(p.second) {
                    it = p.first;
                    it_a = a;
                    valid = 1;
                }
                end = clock();
                t0 += end - start;
                break;
            }
            case 1:
                ++cnt1;
                a=read(); b=read();
                start=clock();
                if (valid && it_a == a && *it == b) valid = 0;
                s[a].erase(b);
                end=clock();
                t1 += end-start;
                break;
            case 2:
                ++cnt2;
                a=read();
                start=clock();
                s[++lst] = s[a];
                end=clock();
                t2+=end-start;
                break;
            case 3: {
                ++cnt3;
                a=read(); b=read();
                start=clock();
                auto it2 = s[a].find(b);
                if (it2 != s[a].end()) {
                    it = it2;
                    it_a = a;
                    valid = 1;
                }
                end=clock();
                t3+=end-start;
                break;
            }
            case 4:
                ++cnt4;
                start=clock();
                a=read(); b=read(); c=read();
                count_elements_in_range(s[a], b, c);
                end=clock();
                t4+=end-start;
                break;
            case 5:
                ++cnt5;
                start=clock();
                if (valid){
                    auto it2=it;
                    if (it==--it2) valid = 0;
                }
                if (valid) auto a = *(--it);
                end=clock();
                t5+=end-start;
                break;
            case 6:
                ++cnt6;
                start=clock();
                if (valid) {
                    auto it2=++it;
                    if (it==++it2) valid = 0;
                    else auto a = *it;
                }
                end=clock();
                t6+=end-start;
                break;
        }
    }    

    alle = clock();
    int allt = alle-alls;
    printf("whole time: %d us\n ", allt);
    printf("emplace: %d us %d\n erase: %d us %d\n = : %d us %d\n find: %d us %d\n range: %d us %d\n --it: %d us %d\n ++it: %d us %d\n ", t0,cnt0,t1,cnt1,t2,cnt2,t3,cnt3,t4,cnt4,t5,cnt5,t6,cnt6);
    ll cntL=0, cntR=0;
    //for(int i=0;i<=25;i++) cntL+=s[i].cntRL, cntR+=s[i].cntRR;
    //for(int i=0;i<=25;i++) cntL+=s[i].cntM, cntR+=s[i].cntS;
    //printf("rotate left: %lld, rotate right: %lld\n", cntL, cntR);
    //printf("merge: %lld, split: %lld\n", cntL, cntR);
    return 0;
}
