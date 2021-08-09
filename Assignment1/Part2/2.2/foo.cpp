#include <bits/stdc++.h>

using namespace std;

typedef long long ll;
typedef long double ld;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;
typedef vector<int> vi;
typedef vector<ll> vl;
typedef vector<pii> vii;
typedef vector<vi> vvi;
typedef vector<pll> vll;
typedef vector<vl> vvl;

#define fori(i, n) for (int i = 0; i < n; i++)
#define ford(i, n) for (int i = n - 1; i >= 0; i--)
#define rep(i, a, b) for (int i = a; i <= b; i++)
#define repd(i, a, b) for (int i = a; i >= b; i--)
#define trav(x, a) for (auto &x : a)
#define all(x) (x).begin(), (x).end()
#define pb push_back
#define eb emplace_back
#define endl '\n'
#define sz(a) (int)(a).size()
#define fi first
#define se second

clock_t time_p = clock();
void time_taken()
{
    time_p = clock() - time_p;
    cerr << "Time Taken : " << (float)(time_p) / CLOCKS_PER_SEC << "\n";
}

const ll mod = 1e9 + 7;
const ll INF = 1e18;

int main()
{
    ios_base::sync_with_stdio(false), cin.tie(nullptr);

    int N = 1e3;
    cout << N << endl;
    fori(i,N){
        if(i%2==0){
            cout << "player1" << endl;
        }
        else{
            cout << "player2" << endl;
        }
    }

    time_taken();
    return 0;
}