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

struct Edge{
    int from, to, weight;
};

struct GroundSetElement{
    bool in_independent_set;
    int from, to, weight;
};

vector<GroundSetElement> ground_set;

vector<pair<int, int>> EL, dist(1540);
vector<vector<int>> AdjMat;
vector<int> max_deg, init_deg, degree(55);

bitset<1540> safe1, safe2;
int weight[1540], before[1540];

DSU init_DSU, graphic(55);

int N, K, answer = 1e9;

bool graphic_check(int u, int v){
    return graphic.rep(u) != graphic.rep(v);
}

bool degree_check(int u, int v){
    return degree[u] > 0 && degree[v] > 0;
}

void prepare(){
    for(int i = 0; i < N; ++i){
        graphic.sz[i]  = init_DSU.sz[i];
        graphic.par[i] = init_DSU.par[i];
    }

    for(int i = 0; i < N; ++i){
        degree[i] = init_deg[i];
    }

    for(int i = 0; i < ground_set.size(); ++i){
        if(ground_set[i].in_independent_set){
            int u = ground_set[i].from, v = ground_set[i].to;
            graphic.comb(u, v);
            degree[u]--; degree[v]--;
        }
    }
}

bool augment(){
    vector<pair<int, int>>().swap(EL);
    prepare();

    for(int i = 0; i < ground_set.size(); ++i){
        if(!ground_set[i].in_independent_set){
            safe1[i] = graphic_check(ground_set[i].from, ground_set[i].to);
            safe2[i] = degree_check(ground_set[i].from, ground_set[i].to);
            weight[i] = -ground_set[i].weight;
        }
        else{
            safe1[i] = safe2[i] = 0;
            weight[i] = ground_set[i].weight;
        }
        weight[i] *= -1;
    }

    for(int i = 0; i < ground_set.size(); ++i){
        if(ground_set[i].in_independent_set){
            ground_set[i].in_independent_set = 0;
            prepare();
            ground_set[i].in_independent_set = 1;
            for(int j = 0; j < ground_set.size(); ++j){
                if(!ground_set[j].in_independent_set){
                    if(graphic_check(ground_set[j].from, ground_set[j].to)){
                        EL.push_back(make_pair(i, j));
                    }
                    if(degree_check(ground_set[j].from, ground_set[j].to)){
                        EL.push_back(make_pair(j, i));
                    }
                }
            }
        }
    }

    for(int i = 0; i < ground_set.size(); ++i){
        dist[i] = make_pair(1000000000, 1000000000);
        before[i] = -1;
        if(safe1[i]){
            dist[i] = make_pair(weight[i], 0);
        }
    }

    for(bool change = 1; change;){
        change = 0;
        for(auto elem : EL){
            pair<int, int> nd = dist[elem.first];
            if(nd.first == 1000000000){
                continue;
            }
            nd.first += weight[elem.second];
            nd.second++;
            if(dist[elem.second] > nd){
                dist[elem.second] = nd;
                before[elem.second] = elem.first;
                change = 1;
            }
        }
    }

    int index = -1;
    for(int i = 0; i < ground_set.size(); ++i){
        if(safe2[i] && (index == -1 || dist[index] > dist[i])){
            index = i;
        }
    }
    if(index == -1 || dist[index].first == 1000000000){
        return 0;
    }

    while(index != -1){
        ground_set[index].in_independent_set ^= 1;
        index = before[index];
    }
    return 1;
}

int calculate(int need, int init_cost){
    int took = 0;
    while(augment()){
        took++;
        int result = init_cost;
        for(int i = 0; i < ground_set.size(); ++i){
            if(ground_set[i].in_independent_set){
                result += ground_set[i].weight;
            }
        }
        prepare();
        for(int i = 0; i < ground_set.size(); ++i){
            if(!ground_set[i].in_independent_set && graphic.comb(ground_set[i].from, ground_set[i].to)){
                result += ground_set[i].weight;
            }
        }
        if(result >= answer){
            return -1;
        }
    }

    if(took < need){
        return -1;
    }

    int result = init_cost;
    for(int i = 0; i < ground_set.size(); ++i){
        if(ground_set[i].in_independent_set){
            result += ground_set[i].weight;
        }
    }

    return result;
}

vector<Edge> forest_edges;
int cnt_forest = 0;

void build_answer(int pos = 0){
    bool can = 1;
    DSU checker(K);
    for(auto elem : forest_edges){
        if(!checker.comb(elem.from, elem.to)){
            can = 0;
            break;
        }
    }
    if(!can)return;

    if(pos == K){
        init_DSU.build(N);
        init_deg = vector<int>(N, 1e9);
        for(int i = 0; i < K; ++i){
            init_deg[i] = max_deg[i];
        }

        can = 1;
        int need = N - 1, init_cost = 0;
        for(auto elem : forest_edges){
            init_DSU.comb(elem.from, elem.to);
            init_deg[elem.from]--;
            init_deg[elem.to]--;
            init_cost += elem.weight;
            need--;
            if(init_deg[elem.from] < 0 || init_deg[elem.to] < 0){
                can = 0;
                break;
            }
        }
        if(!can)return;

        cnt_forest++;

        for(int i = 0; i < ground_set.size(); ++i){
            ground_set[i].in_independent_set = 0;
        }

        int temp = calculate(need, init_cost);
        if(temp == -1){
            return;
        }
        answer = min(answer, temp);

        return;
    }

    for(int mask = 0; mask < (1 << pos); ++mask){
        int cnt = 0;
        for(int i = 0; i < pos; ++i){
            if(mask >> i & 1){
                cnt++;
                forest_edges.push_back({i, pos, AdjMat[i][pos]});
            }
        }
        build_answer(pos + 1);
        while(cnt--){
            forest_edges.pop_back();
        }
    }
}

bool cmp(const GroundSetElement &p, const GroundSetElement &q){
    return p.weight < q.weight;
}

void solve(){
    cin >> N >> K;

    max_deg = vector<int>(K, 0);
    for(int i = 0; i < K; ++i){
        cin >> max_deg[i];
    }

    AdjMat = vector<vector<int>>(N, vector<int>(N));
    for(int i = 0; i < N - 1; ++i){
        for(int j = i + 1; j < N; ++j){
            cin >> AdjMat[i][j];
        }
    }

    for(int i = 0; i < N; ++i){
        for(int j = i + 1; j < N; ++j){
            if(max(i, j) < K)continue;

            GroundSetElement now;
            now.in_independent_set = 0;
            now.from = i; now.to = j;
            now.weight = AdjMat[i][j];
            ground_set.push_back(now);
        }
    }
    sort(ground_set.begin(), ground_set.end(), cmp);

    build_answer();

    cout << answer;
}

int main(){
    ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);

    int tc = 1; //cin >> tc;

    while(tc--){
        solve();
    }

    return 0;
}
