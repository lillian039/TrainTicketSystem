#ifndef TRAINTICKETSYSTEM_BPTREE_HPP
#define TRAINTICKETSYSTEM_BPTREE_HPP

#include <iostream>

template<class Key, class T, int M = 10, int L = 100>//M 阶BPT 块中数据为L
//B+树维护索引
//文件*2 bptree主体 leaf结构
class BPTree {
private:
    std::fstream file_bpt,file_leaf;
    std::pair <Key, T> value;
    std::string recyle;
    int root;//根节点所在位置
    int empty;//空的空间所在位置，为一个单向链表
    struct Node {
        bool is_leaf;
        int n;// n个关键字
        int A[M + 1];//大于、小于关键字的地址
        std::pair<Key, int> K[M];//关键字 与关键字所存元素所在地址
    };

    struct Leaf {
        int nxt;//顺序查找的下一位地址
        std::pair <Key, T> value[L];
    };
private:
    void spiltL() {}//叶节点分裂

    void mergeL() {}//叶节点合并

    void splitN() {}//节点分裂

    void mergeN() {}//节点合并

public:

    BPT(std::string name) {}
    //打开文件 + 读取根节点与第一层子节点内存中 + 打开空间回收文件

    BPT() = default;

    ~BPT() {}
    //关闭文件 + 将根节点与第一层子节点读入文件

    void clear() {}
    //清空bpt

    void insert(const std::pair <Key, T> &val) {}
    //插入键值+数据

    void remove(const Key &key) {}
    //通过键值删除 并进行空间回收

    T find(const Key &key) {}
    //一对一键值查找

    std::vector <T> Find(const Key &key) {}
    //一对多键值查找

};

template<class T>//索引结构体 找到总数据库
struct Index{
    T key;
    int idx;
};

#endif //TRAINTICKETSYSTEM_BPTREE_HPP
