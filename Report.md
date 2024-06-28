---
date: "2024-05-24"
fontsize: 12pt
mainfont: "Noto Sans CJK SC"
---

# 性能对比：AVL & RBT & 无旋Treap & std::set

## 实验原始数据

使用1.in数据测试运行时间（单位：us）: 

* emplace 998 次，erase 333 次，= 4 次，find 222 次，range 111 次，--it 167 次，++it 167 次

p.s. 根据1.cpp的实现方式，--it和++it，即case5和6，实际执行了两次操作

|  | RBT | AVL | 无旋Treap | std::set |
|:-----:|:----:|:----:|:----:|:----:|
|总时长|1091|1482|1344|1259|
|emplace|259|476|325|375|
|erase|116|168|162|140|
|=|49|50|89|85|
|find|59|48|53|73|
|range|133|143|160|99|
|--it|22|30|33|32|
|++it|24|30|23|29|

### 调整平衡次数

* RBT Rotate_Left: 42, Rotate_Right: 140737349333381 （次）
* AVL Rotate_Left: 6799976246779207283, Rotate_Right: 6800116984128540634 （次）
* Treap Merge: 6799976246779217101, Split: 6800116984128551655 （次）
  
p.s. std::set 没有 range 接口，用以下程序代替:

``` cpp
int count_elements_in_range(const std::set<ll>& my_set, ll low, ll high) {
    auto it_low = my_set.lower_bound(low);
    auto it_high = my_set.upper_bound(high);
    return std::distance(it_low, it_high);
}
```

Notice: 以上数据仅为运行一次程序得到的数据，由于数据量较小，误差较大，运行多次得到的结果存在偏差。以上为人工选取的比较有代表性的一次测试结果。

## 实验结果分析

1. RBT:
   * 在总时长和大部分单项操作中表现优异，尤其是在 emplace 和 erase 操作中性能较好。
2. AVL:
   * 相比于红黑树，总体表现稍差，特别是在 emplace 和 erase 操作中耗时较多。
   * 左旋和右旋次数相对较平衡，但次数非常多，是导致其性能稍差的原因。
   * find 操作中表现最优。因为树高严格为 log.
3. 无旋 Treap:
   * 总时长略优于AVL树，但劣于红黑树。
   * Merge和Split操作次数相对较高，消耗较大。
   * 随机性较大，多次测试中表现很不稳定。
4. std::set:
   * 作为标准库实现，基于红黑树，在性能上略逊于手写的红黑树实现。
   * range 操作表现优异。
   * = 操作常数较大。
  
## 实验小结

* 综合来看，性能上红黑树优于AVL树优于无旋Treap（与我在OJ上提交的测试结果一致）。
* AVL树在查找上具有优势，插入、删除性能不如红黑树。调整平衡消耗极大、次数极多。
* 无旋Treap实现较简单，支持可持久化。但不如红黑树和AVL树稳定，整体性能在数据较大时甚至不如AVL树。