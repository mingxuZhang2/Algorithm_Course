#include<bits/stdc++.h>
using namespace std;
int n;
int man_prefered[50][50];
int woman_prefered[50][50];
bool woman_vis[50];//woman[i]是否有约
int match[50];//match[i]代表第i个女人跟match[i]在进行约会
int man[50];
bool cmp(int k,int l,int r){  //对女性k而言 比较第l个男人和第r个男人哪个对他的吸引力更强 l更强输出1 r更强输出0
    for(int i=1;i<=n;i++)
    if(woman_prefered[k][i]==l) return 1;
    else if(woman_prefered[k][i]==r) return 0;
}
int main(){
    queue<int> q; //队列用于存储还没有匹配的男性
    cin>>n;
    for(int i=1;i<=n;i++)
    for(int j=1;j<=n;j++)
    cin>>man_prefered[i][j];  //man[i][j]代表第i个男人的第j个优先女人
    for(int i=1;i<=n;i++)
    for(int j=1;j<=n;j++)
    cin>>woman_prefered[i][j];//同理 woman[i][j]代表第i个女人的第j个优先男人
    for(int i=1;i<=n;i++) q.push(i);
    while(q.size()!=0){
        int now=q.front();
        int temp;
        for(int i=1;i<=n;i++){
            temp=man_prefered[now][i];
            if(woman_vis[temp]==0){ //目前优先级最高的女人没人约 那就跟他约会
                woman_vis[temp]=1;
                match[temp]=now;  //temp 女人跟now男人约会
                man[now]=temp;
                break;
            }
            else{ //如果目前这个女人有约了 那么选择权就交给女方 让女方进行选择
                int ntr=match[temp];  //被绿的那个男人
                if(cmp(temp,ntr,now)==0){   //temp这个女人要选择当前的男人和他现在的伴侣 显然现在的男人对他吸引力更高
                    match[temp]=now;
                    man[ntr]=0;
                    man[now]=temp;
                    q.push(ntr);
                    break;
                }
                else continue;
            }
        }
        q.pop();
    }
    for(int i=1;i<=n;i++) cout<<man[i]<<" ";
    cout<<endl;
    return 0;
}
/*
5
2 1 4 5 3
4 2 1 3 5  
2 5 3 4 1
1 4 3 2 5  
2 4 1 5 3  
5 1 2 4 3
3 2 4 1 5  
2 3 4 5 1  
1 5 4 3 2  
4 2 5 3 1 

1 3 2 5 4
*/