#include "/home/xxl/projects/ESet/Persistent_Treap.hpp"
#if defined (_UNORDERED_MAP_)  || (defined (_LIST_)) || (defined (_MAP_)) || (defined (_SET_)) || (defined (_UNORDERED_SET_))||(defined (_GLIBCXX_MAP)) || (defined (_GLIBCXX_UNORDERED_MAP))
BOOM :)
#endif
#include<iostream>

int main()
{
    ESet<int> s2;
    ESet<int>::iterator it2 = s2.find(int(0));
    if(it2 != s2.end()) std::cout << "Wrong!" << std::endl;
    auto it =s2.emplace(int(6)).first;
    for(int i=0;i<=15;i++){
        bool b =s2.emplace(int(i)).second;
        if((!b)^(i == 6))std::cout <<"Wrong!"<< std::endl;
    }
    std::cout << (*it) <<" "<< s2.size()<< std::endl;
    // int cnt=0;
    // while(it!=s2.end()){
    //     std::cout<<++cnt<<std::endl;
    //     ++it;
    // }
    auto ti = it;
    while(it != ++ --it2) {
        // auto itt = s2.begin();
        // while(itt!=s2.end()) {
        //     std::cout<<*itt<<std::endl;
        //     ++itt;
        // }
        // std::cout<<1111111<<std::endl;
        // std::cout<<*it<<" ";
        // ++it;
        // s2.erase(*itt);
        // itt=it;
        auto tmp = *(ti++);
        std::cout<<tmp<<std::endl;
        it++;
        s2.erase(tmp);
        // if(ti!=s2.end()) std::cout<<*(ti++)<<" ";
        // s2.erase(*(it++));
        // if(it!=s2.end()) std::cout<<*it<<std::endl;
    }
    std::cout << s2.size()<< std::endl;
    return 0;
}