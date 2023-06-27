#include<bits/stdc++.h>
using namespace std;
struct Node{
    double x;
    double y;
};
int n;
Node node[300050];
Node tmp[300050];
int temp[300050];
inline bool cmp1(Node a,Node b){if(a.x!=b.x) return a.x<b.x;else return a.y<b.y;}
inline bool cmp2(Node a,Node b){return a.y<b.y;}
inline bool cmp3(int a,int b){return node[a].y<node[b].y;}
double calu(Node a,Node b){
    return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y);
}
inline double dfs(int l,int r){
    if(l==r) return 1e18;
    if(r==l+1) return calu(node[l],node[r]);
    int mid=(l+r)/2;
    double d1=dfs(l,mid);
    double d2=dfs(mid+1,r);
    double d3=1e18;
    double minx=min(d1,d2);
    int cnt=0;
    for(int i=l;i<=r;i++){
        if(fabs(node[i].x-node[mid].x)<=min(d1,d2)) temp[++cnt]=i;
    }
    sort(temp+1,temp+1+cnt,cmp3);
    for(int i=1;i<=cnt;i++){
        for(int j=i+1;j<=cnt&&node[temp[i]].y-node[temp[j]].y<=minx;j++){
            d3=min(d3,calu(node[temp[i]],node[temp[j]]));
        }
    }
    return min(minx,d3);
}
int main(){
    cin>>n;
    for(int i=1;i<=n;i++) cin>>node[i].x>>node[i].y;
    sort(node+1,node+1+n,cmp1);
    double ans=dfs(1,n);
    printf("%0.2lf",ans);
    return 0;
}