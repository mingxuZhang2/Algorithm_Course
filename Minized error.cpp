#include<bits/stdc++.h>
using namespace std;
const int maxn = 10050;
double dp[maxn], sumx[maxn], sumy[maxn], sumx2[maxn], sumxy[maxn];
int C,n;
struct Node{
    int x;
    int y;
}node[maxn];
bool cmp(Node a,Node b){
    return a.x<b.x;
}
double calc(int i,int j){
    double s1 = sumxy[j] - sumxy[i-1];
    double s2 = sumx[j] - sumx[i-1];
    double s3 = sumy[j] - sumy[i-1];
    double s4 = sumx2[j] - sumx2[i-1];
    int number=j-i+1;
    double a=1.0000*(number*s1-s2*s3)/(number*s4-s2*s2);
    double b=1.0000*(s3-a*s2)/number;
    double sse=0;
    for(int k=i;k<=j;k++){
        sse+=(node[k].y-a*node[k].x-b)*(node[k].y-a*node[k].x-b);
    }
    return sse;
}
int main(){
    cin>>n>>C;
    for(int i=1;i<=n;i++) cin>>node[i].x>>node[i].y;
    sort(node+1,node+1+n,cmp);
    for(int i=1;i<=n;i++){
        sumx[i] = sumx[i-1] + node[i].x;
        sumy[i] = sumy[i-1] + node[i].y;
        sumx2[i] = sumx2[i-1] + node[i].x * node[i].x;
        sumxy[i] = sumxy[i-1] + node[i].x * node[i].y;
    }
    dp[0]=0;
    for(int j=1;j<=n;j++){
        dp[j]=dp[0]+calc(1,j)+C;
        for(int i=2;i<=j;i++){
            dp[j]=min(dp[j],dp[i-1]+calc(i,j)+C);
        }
    }
    printf("%0.2lf\n",dp[n]);
    return 0;
}
