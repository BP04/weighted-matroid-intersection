#include <bits/stdc++.h>
using namespace std;

#define ll long long
#define ld long double

struct DSU{
    vector<int> sz, par;
    int N;

    DSU(){

    }

    DSU(int _N){
        N = _N + 5;
        sz  = vector<int>(N + 5, 1);
        par = vector<int>(N + 5, 0);
        iota(par.begin(), par.end(), 0);
    }

    void build(int _N){
        N = _N;
        sz  = vector<int>(N + 5, 1);
        par = vector<int>(N + 5, 0);
        iota(par.begin(), par.end(), 0);
    }

    void reset(){
        sz  = vector<int>(N + 5, 1);
        par = vector<int>(N + 5, 0);
        iota(par.begin(), par.end(), 0);
    }

    int rep(int x){
        if(x == par[x]){
            return x;
        }
        return par[x] = rep(par[x]);
    }

    bool comb(int u, int v){
        u = rep(u); v = rep(v);

        if(u != v){
            if(sz[u] > sz[v])swap(u, v);
            sz[v] += sz[u];
            par[u] = v;
            return 1;
        }

        return 0;
    }
};

vector<pair<int, int>> forest_edges;

int count_forest(int N, int pos = 0){
    bool can = 1;
    DSU checker(N);
    for(auto elem : forest_edges){
        if(!checker.comb(elem.first, elem.second)){
            can = 0;
            break;
        }
    }
    if(!can){
        return 0;
    }

    if(pos == N){
        return 1;
    }

    int res = 0;
    for(int mask = 0; mask < (1 << pos); ++mask){
        int cnt = 0;
        for(int i = 0; i < pos; ++i){
            if(mask >> i & 1){
                cnt++;
                forest_edges.push_back(make_pair(i, pos));
            }
        }
        res += count_forest(N, pos + 1);
        while(cnt--){
            forest_edges.pop_back();
        }
    }
    return res;
}

void solve(){
    int N = 7;

    cout << count_forest(N);
}

int main(){
    ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);

    int tc = 1; //cin >> tc;

    while(tc--){
        solve();
    }

    return 0;
}
