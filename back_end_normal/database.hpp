#ifndef TRAINTICKETSYSTEM_DATABASE_HPP
#define TRAINTICKETSYSTEM_DATABASE_HPP

#include <iostream>
#include <fstream>

//索引指向的总数据库
//文件*1 数据库主体
template<class T>
class Database {
private:
    T data;
    std::fstream file;
    int rear;//文件头尾所在位置
    const int head_of_file = sizeof(int);
    std::string database_name;
public:
    Database() = default;

    //初始化 打开文件
    Database(std::string name) {
        database_name = name += "_database";
        file.open(database_name);
        if (!file) {
            rear = 0;
            file.open(database_name, std::ios::out);
            file.close();
            file.open(database_name);
        } else {
            file.seekg(0);
            file.read(reinterpret_cast<char *>(&rear), sizeof(int));
        }
    }

    //析构 关闭文件
    ~Database() {
        file.seekg(0);
        file.write(reinterpret_cast<char *>(&rear), sizeof(int));
    }

    //根据bptree索引搜到的文件位置找到数据
    T find(int idx, int day = 0) {
        file.seekg(head_of_file + (idx + day) * sizeof(T));
        file.read(reinterpret_cast<char *>(&data), sizeof(T));
        return data;
    }

    //修改某位置的条信息
    void modify(const T &val, int idx, int day = 0) {
        file.seekg(head_of_file + (idx + day) * sizeof(T));
        file.write(reinterpret_cast<const char *>(&val), sizeof(T));
    }

    //插入新的数据 返回插入元素的地址
    int insert(const T &value) {
        rear++;
        file.seekg(head_of_file + rear * sizeof(T));
        file.write(reinterpret_cast<const char *>(&value), sizeof(T));
        return rear;
    }

    void clear() {
        rear = 0;
        file.close();
        file.open(database_name, std::ios::out);
        file.close();
        file.open(database_name);
    }
    //插入新的数据组
};


#endif //TRAINTICKETSYSTEM_DATABASE_HPP
