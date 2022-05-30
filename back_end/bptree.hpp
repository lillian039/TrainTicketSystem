#ifndef TRAINTICKETSYSTEM_BPTREE_HPP
#define TRAINTICKETSYSTEM_BPTREE_HPP

#include <iostream>
#include "bufferList.hpp"
template<class Key, class T, int M = 10, int L = 100>//M 阶BPT 块中数据为L M L均为偶数
//B+树维护索引
//文件*2 bptree主体 leaf结构
//Q:一对一键值查找和一对多键值查找如何区分（？

class BPTree {
private:
    std::fstream file_tree, file_leaf;
    int root_tree, root_leaf;//根节点所在位置
    int empty_tree, empty_leaf;//空的空间所在位置，为一个单向链表
    int rear_tree, rear_leaf;//文件尾所在位置
    int sum_data;//数据条数
    const int len_of_head_leaf = 4 * sizeof(int);
    const int len_of_head_node = 3 * sizeof(int);
    struct Node {
        bool is_leaf;
        int pos,n,father;//pos 所在的位置 n个关键字 父节点所在位置
        int A[M + 1];//大于、小于关键字的地址
        std::pair<Key, T> K[M];//关键字 K[n]中记录A[n+1]中的最小元素 保证关键字严格不同？
        int position(){
            return pos;
        }
    };
    struct Leaf {
        int nxt, fro,pos;//顺序查找的上一块/下一位地址 它本身所在的地址
        int n;//n个数据
        std::pair<Key, T> value[L];
        int position(){
            return pos;
        }
    };
    bufferList<Node> node_buffer;
    bufferList<Leaf> leaf_buffer;
    Node p, fa, root;//p 当前节点 fa 父节点 root节点
    Leaf leaf;

private:
    //叶节点分裂
    void spiltL(int now) {
        Leaf tmp;
        int mid = L / 2;
        for (int i = 0; i < mid; i++)tmp.value[i] = leaf.value[i + mid];
        std::pair<Key, T> new_key = leaf.value[mid];
        leaf.n = mid, tmp.n = mid;
        int index_new = 0;
        tmp.nxt = leaf.nxt;
        if (empty_leaf) {
            index_new = empty_leaf;
            file_leaf.seekg(empty_leaf * sizeof(Leaf) + 4 * sizeof(int));
            file_leaf.read(reinterpret_cast<char *>(&empty_leaf), sizeof(int));
        } else index_new = ++rear_leaf;
        tmp.pos=index_new;
        //更新fro
        Leaf nxt;
        int index_next = leaf.nxt;
        if (index_next) {
            std::pair<bool,Leaf> buffer;
            buffer=leaf_buffer.find(index_next);
            if(buffer.first)nxt=buffer.second;
            else {
                file_leaf.seekg(index_next * sizeof(Node)+ len_of_head_leaf);
                file_leaf.read(reinterpret_cast<char *>(&nxt), sizeof(Leaf));
                nxt.fro = index_new;
                buffer=leaf_buffer.insert(nxt);
                if(buffer.first) {
                    file_leaf.seekg(buffer.second.pos*sizeof(Leaf) + len_of_head_leaf);
                    file_leaf.write(reinterpret_cast<char *>(&buffer.second), sizeof(Leaf));
                }
            }
        }
        leaf.nxt = index_new;
        file_leaf.seekg(leaf.pos * sizeof(Leaf) + len_of_head_leaf);
        file_leaf.write(reinterpret_cast<char *>(&leaf), sizeof(Leaf));
        file_leaf.seekg(index_new * sizeof(Leaf) + len_of_head_leaf);
        file_leaf.write(reinterpret_cast<char *>(&tmp), sizeof(Leaf));
        //至此叶子裂块完毕，开始修改索引树
        //索引树的叶节点
        p.n++;
        for (int i = p.n + 1; i > now + 1; i--)p.A[i] = p.A[i - 1];
        p.A[now + 1] = index_new;
        for (int i = p.n; i > now; i--)p.K[i] = p.K[i - 1];
        p.K[now] = new_key;
        if (p.n == M)splitN();
    }

    //节点分裂
    void splitN() {
        while (p.n == M) {
            //先创建新的块
            Node new_node;
            int new_index;
            std::pair<Key, T> new_key;
            for (int i = 0; i < M / 2; i++)new_node.A[i] = p.A[i + M / 2];
            for (int i = 0; i < M / 2 - 1; i++)new_node.K[i] = p.K[i + M / 2];
            new_node.n = M / 2, p.n = M / 2, new_key = p.K[M / 2 - 1];
            if (empty_tree) {
                new_index = empty_tree;
                file_tree.seekg(empty_tree * sizeof(Node) + len_of_head_node);
                file_tree.read(reinterpret_cast<char *>(&empty_tree), sizeof(int));
            } else new_index = ++rear_tree;
            //-----更新fa的相关信息 获得更新完毕后的fa-----
            //若已经是根了 还得再建一个根
            if (!p.father) {
                Node new_root;
                new_root.n = 2, new_root.father = 0;
                new_root.A[0] = p.pos, new_root.A[1] = new_index;
                new_root.K[0] = new_key;
                new_root.is_leaf = false;
                int root_index = 0;
                if (empty_tree) {
                    root_index = empty_tree;
                    file_tree.seekg(empty_tree * sizeof(Node) + len_of_head_node);
                    file_tree.read(reinterpret_cast<char *>(&empty_tree), sizeof(int));
                } else root_index = ++rear_tree;
                new_root.pos=root_index;
                fa = new_root;
            }
            else {
                std::pair<bool,Node>buffer;
                buffer=node_buffer.find(p.father);
                if(buffer.first)fa=buffer.second;
                else{
                    file_leaf.seekg(p.father * sizeof(Node) + len_of_head_node);
                    file_leaf.read(reinterpret_cast<char *>(&fa), sizeof(Node));
                    buffer=node_buffer.insert(fa);
                    if(buffer.first){
                        file_leaf.seekg(buffer.second.pos * sizeof(Node) + len_of_head_node);
                        file_leaf.write(reinterpret_cast<char *>(&buffer.second), sizeof(Node));
                    }
                }
                int pos= binary_search_node(p.K[0],fa);//todo 暂时不知道会搜到哪个位置可能是pos+1吧后期再调整qnqq琦琦没有多的笔
                for(int i=p.n;i>=pos;i--)p.A[i]=p.A[i-1],p.K[i]=p.K[i-1];//todo K应该会少一个但现在没有草稿纸就先这样
                //todo 调整节点的位置
            }
            new_node.pos=new_index,new_node.father=fa.pos;
            std::pair<bool,Node>buffer;
            buffer=node_buffer.insert(p);
            if(buffer.first){
                file_tree.seekg(buffer.second.pos*sizeof(Node)+len_of_head_node);
                file_tree.write(reinterpret_cast<char *>(&buffer.second), sizeof(Node));
            }
            buffer=node_buffer.insert(new_node);
            if(buffer.first){
                file_tree.seekg(buffer.second.pos*sizeof(Node)+len_of_head_node);
                file_tree.write(reinterpret_cast<char *>(&buffer.second), sizeof(Node));
            }
            p=fa;
        }
    }

    //叶节点合并
    void mergeL() {}

    //todo 节点调整和二分有待考证

    //节点合并
    void mergeN() {}

public:

    //打开文件 + 将一部分内容写进内存
    BPTree(std::string name) {
        std::string file_tree_name, file_leaf_name;
        file_tree_name = name + "_file_tree", file_leaf_name = name + "_file_leaf";
        file_tree.open(file_tree_name);
        file_leaf.open(file_leaf_name);
        if (!file_leaf || !file_tree) { //第一次打开文件 要新建文件 初始化一些东西进去
            file_tree.open(file_tree_name, std::ios::out);
            file_leaf.open(file_leaf_name, std::ios::out);
            int num = 0;           //所有节点的地址都是0_base的
            Node t_n;            //初始化一个根，一个叶子
            t_n.is_leaf = 1, t_n.n = 0, t_n.A[0] = 0,t_n.pos=1;//根节点放在1
            Leaf l_n;
            l_n.nxt = 0, l_n.fro = 0, l_n.n = 0,l_n.pos=0;
            file_leaf.seekg(0);
            file_tree.seekg(0);
            for (int i = 1; i <= 4; i++)file_leaf.write(reinterpret_cast<char *>(&num), sizeof(int));//初始化叶子
            file_leaf.write(reinterpret_cast<char *>(&l_n), sizeof(Leaf));
            num = 1;
            for (int i = 1; i <= 2; i++)file_tree.write(reinterpret_cast<char *>(&num), sizeof(int));//初始化索引树
            num = 0;
            file_tree.write(reinterpret_cast<char *>(&num), sizeof(int));
            file_tree.seekg(len_of_head_node+ sizeof(Node));
            file_tree.write(reinterpret_cast<char *>(&t_n), sizeof(Node));
            file_tree.close();
            file_leaf.close();
            file_tree.open(file_tree_name);
            file_leaf.open(file_leaf_name);
        }
        file_tree.seekg(0), file_leaf.seekg(0);
        file_tree.read(reinterpret_cast<char *>(&root_tree), sizeof(int));
        file_tree.read(reinterpret_cast<char *>(&empty_tree), sizeof(int));
        file_tree.read(reinterpret_cast<char *>(&rear_tree), sizeof(int));
        file_tree.seekg(len_of_head_node+ root_tree*sizeof(Node));
        file_tree.read(reinterpret_cast<char *>(&root), sizeof(Node));
        file_leaf.read(reinterpret_cast<char *>(&root_leaf), sizeof(int));
        file_leaf.read(reinterpret_cast<char *>(&empty_leaf), sizeof(int));
        file_leaf.read(reinterpret_cast<char *>(&rear_leaf), sizeof(int));
        file_leaf.read(reinterpret_cast<char *>(&sum_data), sizeof(int));
    }

    //关闭文件 + 将内存里头的东西写进文件
    ~BPTree() {
        file_tree.seekg(0), file_leaf.seekg(0);
        file_tree.write(reinterpret_cast<char *>(&root_tree), sizeof(int));
        file_tree.write(reinterpret_cast<char *>(&empty_tree), sizeof(int));
        file_tree.write(reinterpret_cast<char *>(&rear_tree), sizeof(int));
        file_tree.seekg(root.pos);
        file_tree.write(reinterpret_cast<char*>(&root), sizeof(Node));
        while(!node_buffer.empty()){
            Node buffer=node_buffer.pop_back();
            file_tree.seekg(buffer.pos* sizeof(Node)+len_of_head_node);
            file_tree.write(reinterpret_cast<char *>(&buffer), sizeof(Node));
        }
        file_tree.seekg(len_of_head_node+ root_tree*sizeof(Node));
        file_tree.write(reinterpret_cast<char *>(&root), sizeof(Node));
        file_leaf.write(reinterpret_cast<char *>(&root_leaf), sizeof(int));
        file_leaf.write(reinterpret_cast<char *>(&empty_leaf), sizeof(int));
        file_leaf.write(reinterpret_cast<char *>(&rear_leaf), sizeof(int));
        file_leaf.write(reinterpret_cast<char *>(&sum_data), sizeof(int));
        while(!leaf_buffer.empty()){
            Leaf buffer=leaf_buffer.pop_back();
            file_tree.seekg(buffer.pos* sizeof(Node)+len_of_head_leaf);
            file_tree.write(reinterpret_cast<char *>(&buffer), sizeof(Leaf));
        }
        file_tree.close();
        file_leaf.close();
    }

    //清空bpt 虽然我暂时不知道什么时候需要清空
    void clear() {
    }

    //插入键值+数据
    bool insert(const std::pair<Key, T> &val) {
        p = root;
        int index_node, index_leaf;
        while (!p.is_leaf) {
            int now = 0;
            while (now < p.n && val >= p.K[now])now++;//K[n]中记录A[n+1]中的最小元素
            std::pair<bool,Node> buffer=node_buffer.find(p.A[now]);
            if(buffer.first)p=buffer.second;
            else{
                index_node = p.A[now] * sizeof(Node) + 3 * sizeof(int);
                file_tree.seekg(index_node);
                file_tree.read(reinterpret_cast<char *>(&p), sizeof(Node));
                buffer=node_buffer.insert(p);
                if(buffer.first){
                    file_tree.seekg(buffer.second.pos* sizeof(Node)+len_of_head_node);
                    file_tree.write(reinterpret_cast<char *>(&buffer.second), sizeof(Node));
                }
            }
        }
        int now = binary_search_node(val,p);
       // while (now < p.n && val >= p.K[now])now++;//K[n]中记录A[n+1]中的最小元素
        std::pair<bool,Leaf> buffer=leaf_buffer.find(p.A[now]);
        if(buffer.first)leaf=buffer.second;
        else {
            index_leaf = p.A[now] * sizeof(Leaf) + 4 * sizeof(int);
            file_leaf.seekg(index_leaf);
            file_leaf.read(reinterpret_cast<char *>(&leaf), sizeof(Leaf));
            buffer=leaf_buffer.insert(leaf);
            if(buffer.first){
                file_leaf.seekg(buffer.second.pos*sizeof (Leaf)+len_of_head_leaf);
                file_leaf.write(reinterpret_cast<char *>(&buffer.second), sizeof(Leaf));
            }
        }
        int pos = binary_search_leaf(val);
        if (leaf.n && leaf.value[pos] == val)return false;
        leaf.n++;
        sum_data++;
        for (int i = leaf.n; i > pos; i--)leaf.value[i] = leaf.value[i - 1];
        leaf.value[pos] = val;
        if (leaf.n == L)spiltL(now);
            //然后开始判分块
        else leaf_buffer.insert(leaf);
        return true;
    }

    //返回数据条数
    int size() {
        return sum_data;
    }

    //通过键值删除 并进行空间回收
    void remove(const std::pair<Key,T>&val) {}

    //支持一对多键值查找
    std::pair<bool,std::vector<T>> find(const Key &key) {
        std::pair<bool,std::vector<T>> ans;
        p = root;
        int index_node, index_leaf;
        while (!p.is_leaf) {
            int now = 0;
            while (now < p.n && key >= p.K[now].first)now++;//K[n]中记录A[n+1]中的最小元素
            index_node = p.A[now] * sizeof(Node) + 3 * sizeof(int);
            file_tree.seekg(index_node);
            file_tree.read(reinterpret_cast<char *>(&p), sizeof(Node));
        }
        int now = 0;
        while (now < p.n && key >= p.K[now].first)now++;//K[n]中记录A[n+1]中的最小元素
        index_leaf = p.A[now] * sizeof(Leaf) + 3 * sizeof(int);
        file_tree.seekg(index_leaf);
        file_tree.read(reinterpret_cast<char *>(&leaf), sizeof(Leaf));
        int pos = binary_search_key(key);
        while (pos >= 0 && leaf.value[pos].first == key)ans.second.push_back(leaf.value[pos--].second);
        if(ans.second.empty())ans.first=false;
        else ans.first=true;
        return ans;
    }


    void traverse() {
        leaf.nxt = root_leaf;
        while (leaf.nxt) {
            std::pair<bool,Leaf> buffer=leaf_buffer.find(leaf.nxt);
            if(buffer.first)leaf=buffer.second;
            else {
                file_leaf.seekg(leaf.nxt * sizeof(Leaf) + len_of_head_leaf);
                file_leaf.read(reinterpret_cast<char *>(&leaf), sizeof(Leaf));
                buffer=leaf_buffer.insert(leaf);
                if(buffer.first){
                    file_leaf.seekg(buffer.second.pos * sizeof(Leaf) + len_of_head_leaf);
                    file_leaf.read(reinterpret_cast<char *>(&buffer.second), sizeof(Leaf));
                }
            }
            for (int i = 0; i < leaf.n; i++)std::cout << leaf.value[i].first<<" "<<leaf.value[i].second << "    ";
            std::cout << std::endl;
        }
    }

private:
    //todo 这些二分搜索都不知道是不是对的而且似乎都没写完
    int binary_search_leaf(const std::pair<Key, T> &val) {
        int l = 0, r = leaf.n - 1;
        while (l < r) {
            int mid = (l + r + 1) / 2;
            if (leaf.value[mid] >= val)r = mid - 1;
            else l = mid;
        }
        return l;
    }

    int binary_search_key(const Key &key){//?
        int l = 0, r = leaf.n - 1;
        while (l < r) {
            int mid = (l + r + 1) / 2;
            if (leaf.value[mid].first >= key)r = mid - 1;
            else l = mid;
        }
        return l;
    }

    int binary_search_node(const std::pair<Key,T>&val,const Node &node){
        int l = 0, r =  node.n-1;
        while (l < r) {
            int mid = (l + r + 1) / 2;
            if (node.K[mid] >= val)r = mid - 1;
            else l = mid;
        }
        return l;
    }
};


#endif //TRAINTICKETSYSTEM_BPTREE_HPP
