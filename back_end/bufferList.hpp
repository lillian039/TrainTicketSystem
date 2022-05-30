#ifndef BPTREE_HPP_BUFFERLIST_HPP
#define BPTREE_HPP_BUFFERLIST_HPP

template<class T,int L=50>
class bufferList{
private:
    int size=0;
    struct linkNode{
        T val;
        linkNode *nxt,*fro;
        linkNode(const T &tmp,linkNode *nxt_= nullptr,linkNode *fro_= nullptr){
            val=tmp,nxt=nxt_,fro=fro_;
        }
    };
    linkNode *head, *rear;
public:
    bufferList(){
        size=0;
        head= rear=nullptr;
    }

    ~bufferList(){
        linkNode *p=head,*del;
        while(p){
            del=p,p=p->nxt;
            delete del;
        }
    }

    //查找是否存在相关元素
    std::pair<bool,T> find(int pos){
        linkNode* p=head;
        while(p){
            if(pos==p->val.position())return std::make_pair(true,p->val);
            p=p->nxt;
        }
        T tmp;
        return std::make_pair(false,tmp);
    }

    //插入元素 并判断有没有需要写入文件的节点弹出
    std::pair<bool,T> insert(const T &val){//队伍中可能有重复的已经修改过了的节点 这个直接废掉即可
        if(!size){
            size++;
            head=rear=new linkNode(val);
            return std::make_pair(false,val);
        }
        head=new linkNode(val,head->nxt);
        if(head->nxt)head->nxt->fro=head;
        size++;
        if(size==L) {
            size--;
            linkNode* del=rear;
            rear=rear->fro,rear->nxt= nullptr;
            std::pair<bool,T> tmp= find(del->val.position());
            if(tmp.first)tmp.first=false;
            else tmp.second=del->val;
            delete del;
            return tmp;
        }
        return std::make_pair(false,val);
    }

    //判断bufferlist是否为空
    bool empty(){
        return size==0;
    }

    //弹出需要写入的节点
    T pop_back(){
        while (true) {
            size--;
            linkNode *del = rear;
            rear = rear->fro;
            if(rear)rear->nxt = nullptr;
                std::pair<bool, T> tmp = find(del->val.position());
            if (tmp.first)delete del;
            else{
                T val=del->val;
                delete del;
                return val;
            }
        }
    }

};
#endif //BPTREE_HPP_BUFFERLIST_HPP
