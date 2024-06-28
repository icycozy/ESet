#include "/home/xxl/projects/ESet/AVL_copy.hpp"
#if defined (_UNORDERED_MAP_)  || (defined (_LIST_)) || (defined (_MAP_)) || (defined (_SET_)) || (defined (_UNORDERED_SET_))||(defined (_GLIBCXX_MAP)) || (defined (_GLIBCXX_UNORDERED_MAP))
BOOM :)
#endif
#include<iostream>
#define ll long long

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
    freopen("1.in", "r", stdin);
    srand(0);
    clock_t start, end;
    start = clock();
    AVL_Set<long long> s[25];
    AVL_Set<long long>::iterator it;
    long long it_a;
    int op, lst=0, valid = 0,cnt=1;
    while (scanf("%d", &op) != EOF) {
        long long a, b, c;
        switch (op) {
            case 0: {
                a=read(); b=read();
                auto p=s[a].emplace(b);
                if(p.second) {
                    it = p.first;
                    it_a = a;
                    valid = 1;
                }
                break;
            }
            case 1:
                a=read(); b=read();
                if (valid && it_a == a && *it == b) valid = 0;
                s[a].erase(b);
                break;
            case 2:
                a=read();
                s[++lst] = s[a];
                break;
            case 3: {
                a=read(); b=read();
                auto it2 = s[a].find(b);
                if (it2 != s[a].end()) {
                    printf("true\n");
                    it = it2;
                    it_a = a;
                    valid = 1;
                } else
                    printf("false\n");
                cnt++;
                break;
            }
            case 4:
                a=read(); b=read(); c=read();
                write(s[a].range(b, c));
                cnt++;
                break;
            case 5:
                if (valid){
                    auto it2=it;
                    if (it==--it2) valid = 0;
                }
                if (valid)
                    write(*(--it));
                else
                    printf("-1\n");
                cnt++;
                break;
            case 6:
                if (valid) {
                    auto it2=++it;
                    if (it==++it2)valid = 0;
                    else write(*it);
                }
                if (!valid)
                    printf("-1\n");
                cnt++;
                break;
        }
    }    
    end = clock(); //程序结束用时
    double endtime = (double) (end - start) / CLOCKS_PER_SEC;
    printf("time=%lf s\n", endtime);
    return 0;
}
