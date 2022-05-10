#ifndef TRAINTICKETSYSTEM_DATABASE_HPP
#define TRAINTICKETSYSTEM_DATABASE_HPP

//索引指向的总数据库
//文件*1 数据库主体
template<class T>
class Database {
private:
    T data;
    fstream file,file_recycle;
    int head,rear;//文件头尾所在位置
    int empty//空的空间所在位置 为一个单向链表
public:
    Database() = default;

    Database(std::string name) {}
    //初始化 打开文件

    ~Database(){}
    //析构 关闭文件

    T find(int idx) {}
    //根据bptree索引搜到的文件位置找到数据

    T find(int idx,int day) {}
    //用于查找某车次车票某天的售卖情况 idx为某车次的车票所在位置

    void remove(int idx){}
    //根据bptree索引搜到的文件位置找到数据 并进行空间回收

    void insert(T value,int time=0){}
    //插入新的数据组
};


#endif //TRAINTICKETSYSTEM_DATABASE_HPP
