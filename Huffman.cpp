#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <cstdio>

#define MAXN 10050
using namespace std;
int n;
int num[MAXN];
struct huffman_node{
    int num;   //num为element出现的次数
    int element;
    huffman_node *leftson;
    huffman_node *rightson;
    huffman_node(){
        num=0;
    }
    huffman_node(int a,int b){
        this->num=a;
        this->element=b;
    }
    huffman_node(huffman_node *l,huffman_node *r){
        this->leftson=l;
        this->rightson=r;
        this->num=l->num+r->num;
        element=0;
    }
};
int qwq;
class Compare_Node_Pointer{
    public:
        bool operator () (huffman_node* &a, huffman_node* &b) const{/* Node::priority 大的优先 */
            return a->num>b->num;
        }
};
priority_queue<huffman_node*,std::vector<huffman_node*>,Compare_Node_Pointer> q;
template<class T>
class Huffman{
    public:
        Huffman();
        ~Huffman(){

        }
        inline void build_tree();
    huffman_node *root;
};
template<class T>
Huffman<T>::Huffman(){
    root=NULL;
}
template<class T>
void Huffman<T>::build_tree(){
    for(int i=1;i<=n;i++){
        huffman_node *node=new huffman_node(num[i],i);q.push(node);
    }
    while(q.size()!=1){
        huffman_node *left=q.top();q.pop();
        huffman_node *right=q.top();q.pop();
        huffman_node *fa=new huffman_node(left,right);
        q.push(fa);
        if(q.size()==1) break;
    } 
    this->root=q.top();
}
string str;
Huffman<int> h;
int deep[100050];
void dfs(huffman_node *now, string code){
    if(now->element != 0){
        deep[now->element] = code.length();
        return;
    }
    dfs(now->leftson, code + "0");
    dfs(now->rightson, code + "1");
}
int main(){
    freopen("1.in","r",stdin);
    freopen("1.txt","w",stdout);
    cin>>n;
    for(int i=1;i<=n;i++) cin>>num[i];
    h.build_tree();
    dfs(h.root,"");
    long long tot=0;
    double avr=0.00;
    for(int i=1;i<=n;i++) tot+=num[i];
    for(int i=1;i<=n;i++) avr+=1.0000*deep[i]*num[i]/tot; 
    printf("%0.2lf",avr);
}