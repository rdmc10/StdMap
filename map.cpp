#include <iostream>
#include <fstream>
#include <queue>
#include <fstream>
#include <vector>

template <class Key, class T,class Compare=std::less<Key>>
class Map{
    private:
        struct Node{
            std::pair<Key,T> el;
            Node* lchild;
            Node* rchild;
            Node* p;
            int height;
            Key first(){return this->el.first;}
            T second(){return this->el.second;}
        };
        Compare comp;
        Node* root;
        unsigned int size;
    public:
        class iterator{
            Node* curr;
            public:
                Key first(){
                    if(curr)return curr->el.first;
                }
                T second(){
                    if(curr)return curr->el.second;
                }
                iterator& operator++(){
                    this->curr = succesor(this->curr);
                    return *this;
                }
                iterator& operator--(){
                    this->curr = predecesor(this->curr);
                    return *this;
                }
                iterator& operator++(int){
                    iterator *temp = this;
                    ++*this;
                    return *temp;
                }
                iterator& operator--(int){
                    iterator *temp = this;
                    --*this;
                    return *temp;
                }
                iterator& operator=(Node* b){
                    this->curr = b;
                    return *this;
                }
                bool operator==(Node* a){
                    if(!a && !curr)return 1;
                    if(!a && curr)return 0;
                    if(a->el == this->curr->el)return 1;
                    return 0;
                }
                bool operator!=(Node* a){
                    if(!a && !curr)return 0;
                    if(!a && curr)return 1;
                    if(a->el != this->curr->el)return 1;
                    return 0;
                }
        };
        Map();
        Node* begin(){return ab_min(root);}
        Node* end(){return nullptr;}
        Node* get_root(){return this->root;}
        int get_size(){return this->size;}
        int fb(Node* x);
        int height(Node* x);
        void insert(Key key,T value);
        void insert_repair(Node* x);
        void transplant(Node* u, Node* v);
        Node* ab_delete(Node *z);
        static Node* ab_min(Node* x);
        static Node* ab_max(Node* x);
        static Node* succesor(Node* x);
        static Node* predecesor(Node* x);
        Node* find(Key key);
        void DELETE(Key key);
        void erase(Node* x);
        void delete_repair(Node* x);
        void rot_st(Node* x);
        void rot_dr(Node* x);
        void print_pre(Node* x);
        void print_in(Node* x);
        void print_post(Node* x);
        void print_tree(int option);
        void recalc_height(Node* x);
        void clear();
        bool empty(){return this->size == 0;}
        int get_height(Node* x){return x->height;}
        void construct(std::vector<std::pair<Key,T>> vec);
        T& operator[](Key key);
        Node* copy_tree(Node* );
        Map& operator=(Map b);
};

template <class Key,class T,class Compare>
void Map<Key,T,Compare>::erase(Node* x){
    if(x){
        Node* rebalance =ab_delete(x);
        delete_repair(rebalance);
        size--;
    }
}

template <class Key,class T,class Compare>
void Map<Key,T,Compare>::clear(){
    while(size){
        ab_delete(root);
        size--;
    }
}

template<class Key,class T,class Compare>
Map<Key,T,Compare>& Map<Key,T,Compare>::operator=(Map b){
    if(b.root == nullptr)return *this;

    this->root = copy_tree(b.root);
    this->size = b.size;

    return *this;
}

template<class Key,class T,class Compare>
typename Map<Key,T,Compare>::Node* Map<Key,T,Compare>::copy_tree(Node* root)
{
    Node* copyNode = nullptr;
    if (root){
        copyNode = new Node;
        copyNode->el = root->el;
        copyNode->height = root->height;
        copyNode->lchild = copy_tree(root->lchild);
        copyNode->rchild = copy_tree(root->rchild);
        if(copyNode->lchild)
            copyNode->lchild->p = copyNode;
        if(copyNode->rchild)
            copyNode->rchild->p = copyNode;
    }
    return copyNode;
}

template <class Key,class T, class Compare>
T& Map<Key,T,Compare>::operator[](Key key){
    Node* found = this->find(key);
    if(found){
        return found->el.second;
    }else{
        this->insert(key,0);
        return find(key)->el.second;
    }
}

template <class Key,class T, class Compare>
void Map<Key,T,Compare>::recalc_height(Node* x){
    Node* temp = x;
    while(temp){
        temp->height = this->height(temp);
        temp=temp->p;
    }
}

template <class Key,class T, class Compare>
void Map<Key,T,Compare>::print_tree(int option){
    if(option == 0)print_pre(root);
    else if(option == 1)print_in(root);
    else if(option == 2)print_post(root);
}

template <class Key,class T, class Compare>
void Map<Key,T,Compare>::print_pre(Node* x){
    if(x){
        std::cout << x->el.first << "," << x->height << std::endl;
        print_pre(x->lchild);
        print_pre(x->rchild);
    }
}


template <class Key,class T, class Compare>
void Map<Key,T,Compare>::print_in(Node* x){
    if(x){
        print_in(x->lchild);
        std::cout << x->el.first << "," << x->height << std::endl;
        print_in(x->rchild);
    }
}

template <class Key,class T, class Compare>
void Map<Key,T,Compare>::print_post(Node* x){
    if(x){
        print_post(x->lchild);
        print_post(x->rchild);
        std::cout << x->el.first << "," << x->height << std::endl;
    }
}

template <class Key,class T, class Compare>
typename Map<Key,T,Compare>::Node* Map<Key,T,Compare>::find(Key key){
    Node* t = root;
    while(t){
        if(t->el.first == key)break;
        else if(comp(key,t->el.first))t=t->lchild;
        else t=t->rchild;
    }
    return t;
}

template <class Key,class T, class Compare>
Map<Key,T,Compare>::Map(){
    root = nullptr;
    size = 0;
}

template <class Key,class T, class Compare>
int Map<Key,T,Compare>::fb(Node* x){
    int hl,hr;
    hl=x && x->lchild?x->lchild->height:-1;
    hr=x && x->rchild?x->rchild->height:-1;
    return hr-hl;
}

template <class Key,class T, class Compare>
int Map<Key,T,Compare>::height(Node* x){
    int hl,hr;
    hl =x && x->lchild?x->lchild->height:-1;
    hr =x && x->rchild?x->rchild->height:-1;
    return hl>hr?hl+1:hr+1;
}

template <class Key,class T, class Compare>
void Map<Key,T,Compare>::transplant(Node* u, Node* v){
    if(u->p == nullptr)root = v;
    else{
        if(u == u->p->lchild)u->p->lchild = v;
        else u->p->rchild = v;
    }
    if(v != nullptr)v->p=u->p;
    recalc_height(v);
}

template <class Key,class T, class Compare>
typename Map<Key,T,Compare>::Node* Map<Key,T,Compare>::ab_delete(Node* z){
    Node* rebalance;
    if(z->lchild == nullptr){
        transplant(z,z->rchild);
        rebalance = z->p;
        delete z;
    }
    else{
        if(z->rchild==nullptr){
            transplant(z,z->lchild);
            rebalance=z->p;
            delete z;
        }
        else{
            Node* y = succesor(z);
            if(y->p!=z)
                rebalance = y->p;
            else rebalance = y;
            if(y!=z->rchild){
                transplant(y,y->rchild);
                y->rchild=z->rchild;
                z->rchild->p=y;
            }
            transplant(z,y);
            y->lchild = z->lchild;
            z->lchild->p=y;
            delete z;
        }
    }
    recalc_height(rebalance);
    return rebalance;
}

template <class Key,class T, class Compare>
void Map<Key,T,Compare>::insert(Key key,T value){
    Node* temp = new Node;
    temp->el = std::make_pair(key,value);
    temp->lchild = nullptr;
    temp->rchild = nullptr;
    temp->p=nullptr;
    temp->height=0;
    Node* y = nullptr;
    if(root == nullptr)root = temp;
    else{
        Node* x = root;

        while (x != nullptr) {
            y = x;
            if(key == x->el.first){
                x->el.second = value;
                break;
            }
            else if(comp(key,x->el.first)){
                x=x->lchild;
            }else
                x=x->rchild;
        }
        temp->p = y;
        if(comp(key,y->el.first))
            y->lchild = temp;
        else
            y->rchild = temp;
    }
    recalc_height(temp);
    insert_repair(temp);
    size++;
}

template <class Key,class T, class Compare>
void Map<Key,T,Compare>::insert_repair(Node* temp){
    temp = temp->p;
    while(temp){
        if(fb(temp) == 0)break;
        if(fb(temp) == -1 || fb(temp) == 1)
            temp = temp->p;
        else {
            if(fb(temp) == -2 && fb(temp->lchild) == -1)rot_dr(temp);
            else if(fb(temp) == 2 && fb(temp->rchild) == 1)rot_st(temp);
            else if(fb(temp) == -2 && fb(temp->lchild) == 1){
                rot_st(temp->lchild);
                rot_dr(temp);
            }else if(fb(temp) == 2 && fb(temp->rchild) == -1){
                rot_dr(temp->rchild);
                rot_st(temp);
            }
            else {
                break;
            };
            recalc_height(temp);
        }
    }
}

template <class Key,class T, class Compare>
void Map<Key,T,Compare>::rot_st(Node* y){
    Node* x = y->rchild;
    y->rchild = x->lchild;
    if(x->lchild)x->lchild->p=y;
    x->p=y->p;
    if(!y->p)root = x;
    else{
        if(y == y->p->lchild)y->p->lchild=x;
        else
            y->p->rchild = x;
    }
    x->lchild=y;
    y->p=x;
    y->height = this->height(y);
    x->height = this->height(x);
}

template <class Key,class T, class Compare>
void Map<Key,T,Compare>::rot_dr(Node* y){
    Node* x = y->lchild;
    y->lchild = x->rchild;
    if(x->rchild)x->rchild->p=y;
    x->p=y->p;
    if(!y->p)root = x;
    else{
        if(y == y->p->rchild)y->p->rchild=x;
        else
            y->p->lchild = x;
    }
    x->rchild=y;
    y->p=x;
    y->height = this->height(y);
    x->height = this->height(x);
}

template <class Key,class T, class Compare>
typename Map<Key,T,Compare>::Node* Map<Key,T,Compare>::ab_min(Node* x){
    if(!x)return nullptr;
    Node* y=x;
    while(y->lchild)
        y=y->lchild;
    return y;
}

template <class Key,class T, class Compare>
typename Map<Key,T,Compare>::Node* Map<Key,T,Compare>::ab_max(Node* x){
    if(!x)return nullptr;
    Node* y=x;
    while(y->rchild)
        y=y->rchild;
    return y;
}

template <class Key,class T, class Compare>
typename Map<Key,T,Compare>::Node* Map<Key,T,Compare>::succesor(Node* x){
    if(!x)return nullptr;
    Node* y=nullptr;
    if(x->rchild)y=ab_min(x->rchild);
    else{
        y=x->p;
        while(y && x == y->rchild){
            x=y;
            y=y->p;
        }
    }
    return y;
}

template <class Key,class T, class Compare>
typename Map<Key,T,Compare>::Node* Map<Key,T,Compare>::predecesor(Node* x){
    if(!x)return nullptr;
    Node* y=nullptr;
    if(x->lchild)y=ab_max(x->lchild);
    else{
        y=x->p;
        while(y && x == y->lchild){
            x=y;
            y=y->p;
        }
    }
    return y;
}

template <class Key,class T, class Compare>
void Map<Key,T,Compare>::DELETE(Key key){
    Node* t = find(key);

    if(t){
        Node* rebalance =ab_delete(t);
        delete_repair(rebalance);
        size--;
    }
}

template <class Key,class T, class Compare>
void Map<Key,T,Compare>::delete_repair(Node* x){
    Node* tmp = x;
    while(tmp){
        if(fb(tmp) == 0)tmp=tmp->p;
        else if(fb(tmp) == 1 || fb(tmp) == -1)break;
        else{
            if(fb(tmp) == -2){
                if(fb(tmp->lchild) == 0){
                    rot_dr(tmp);
                    break;
                }else if(fb(tmp->lchild) == -1){
                    rot_dr(tmp);
                    tmp=tmp->p;
                }else if(fb(tmp->lchild) == 1){
                    rot_st(tmp->lchild);
                    rot_dr(tmp);
                    tmp=tmp->p;
                }
            }else if(fb(tmp) == 2){
                if(fb(tmp->rchild) == 0){
                    rot_st(tmp);
                    break;
                }else if(fb(tmp->rchild) == 1){
                    rot_st(tmp);
                    tmp=tmp->p;
                }else if(fb(tmp->rchild) == -1){
                    rot_dr(tmp->rchild);
                    rot_st(tmp);
                    tmp=tmp->p;
                }
            }
        }
    }
}

template <class Key>
struct cmp{
    bool operator()(Key a,Key b){
        return a>b;
    }
};

int main(){
    Map<std::string,int> mp;
    Map<std::string,int>::iterator it;
    std::ifstream fin("map.in");
    int n;
    fin >> n;
    std::string nume;
    int varsta;
    for(int i=0;i<n;i++){
        fin >> nume >> varsta;
        mp.insert(nume,varsta);
    }

    std::cout << "\n";
    for(it = mp.begin();it!=mp.end();it++)
        std::cout << it.first() << " " <<it.second() << "\n";
    std::cout << "\n";
    mp.insert("Balaur",17);
    mp.DELETE("Adriana");
    mp.DELETE("Mihai");
    mp["Dominic"]=28;
    mp["Alex"]=18;
    mp["Laurentiu"]=38;
    int x = mp["Ionescu"];
    std::cout << x << std::endl;



    for(it = mp.begin();it!=mp.end();it++)
        std::cout << it.first() << " " <<it.second() << "\n";
    std::cout << "\n";
    it=mp.begin();
    it++;
    it++;
    it++;
    it++;
    it++;
    it--;
    it--;
    it--;
    std::cout << it.first();


    return 0;
}
