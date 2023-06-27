#include<bits/stdc++.h>
using namespace std;
struct graphNode{
    int vertex;int dis;
    graphNode *nxt;
    graphNode(int u,int dis){
        this->vertex=u;this->dis=dis;this->nxt=NULL;
    }
    graphNode(int u){
        this->vertex=u;this->dis=0;this->nxt=NULL;
    }
};
class graph{
    public:
        graph(){
            for(int i=0;i<500;i++) g[i]=new graphNode(i);
            num=cnt=0;
        }
        ~graph(){

        }
        inline void tupo_sort();
        inline void bfs();
        inline void add(int i,int j); //建图 用邻接链表的方法
        inline void find_circle();
        inline void output();
        graphNode* g[500];//邻接链表
        int in[5000],out[800];
        int n,m;//图中有n个节点 m条边
        int num,cnt;
        int color[500];
        bool vis[5000];
        int ans[5000];
        int path[500];int numm=0;
};
void graph::add(int i,int j){
    graphNode *current=g[i];
    while(current->nxt!=NULL) current=current->nxt;
    graphNode *newnode=new graphNode(j);
    current->nxt=newnode;
}
void graph::output(){
    bool ju[500]={0};
    int repeat=0;
    int st=0,ed=0;
    for(int i=1;i<=cnt;i++){
    if(ju[ans[i]]==0) ju[ans[i]]=1;
        else if(ju[ans[i]]==1) {repeat=ans[i];break;}
    }
    for(int i=1;i<=cnt;i++) {if(ans[i]==repeat) {st=i;break;}}
    for(int i=cnt;i>=1;i--) {if(ans[i]==repeat) {ed=i;break;}}
    for(int i=st;i<ed;i++) cout<<ans[i]<<",";
    cout<<ans[ed]<<endl;
}
void graph::find_circle(){
    stack<graphNode*> q;
    bool vis[500]={0};
    for(int i=1;i<=n;i++){
        cnt=0;
        graphNode *current=g[i];
        if(vis[current->vertex]==0) q.push(current);
        while(q.size()!=0){
            current=q.top();q.pop();
            ans[++cnt]=current->vertex;
            cout<<"now the node is "<<current->vertex<<endl;
            if(vis[current->vertex]==1&&g[current->vertex]->nxt!=NULL){
                output();
                return ;
            }
            vis[current->vertex]=1;
            current=g[current->vertex];
            if(current->nxt==NULL) cnt--;
            while(current->nxt!=NULL){
                current=current->nxt;
                q.push(current);
            }
            
        }
    }
}
queue<graphNode*>q;
void graph::tupo_sort(){
    for(int i=1;i<=n;i++) if(in[i]==0) q.push(g[i]);
    while(q.size()!=0){
        graphNode *current=q.front();
        ans[++cnt]=current->vertex;num++;
        q.pop();
        current=g[current->vertex];
        while(current->nxt!=NULL){
            current=current->nxt;
            int the_nxt=current->vertex;
            in[the_nxt]--;
            if(in[the_nxt]==0) q.push(current);
        }
    }
    if(num==n){
        cout<<"YES"<<endl;
        for(int i=1;i<cnt;i++) cout<<ans[i]<<",";
        cout<<ans[cnt];
        cout<<endl;
    }
    else{
        cout<<"NO"<<endl;
        find_circle();
    }

}
int main(){
    graph thegraph;
    cin>>thegraph.n>>thegraph.m;
    int x,y;
    for(int i=1;i<=thegraph.m;i++){
        cin>>x>>y;
        thegraph.add(x,y);
        thegraph.in[y]++;
    }
    thegraph.tupo_sort();
    system("pause");
    return 0;
}
/*
6 7
1 2
1 3
2 4
4 5
5 6
6 3
3 2 

expected answer:3 2 4 5 6 3

12 11
1 3
1 2
2 4
2 5
5 11
11 12
2 6
3 7
7 8
8 9
9 6

9 9
1 2 
1 10
2 3
3 4
9 1
4 5
5 7
7 8
3 9

7 7
7 1
3 7
1 2
2 3
3 4
4 5
5 6
expected answer : 1 2 3 7 1
*/