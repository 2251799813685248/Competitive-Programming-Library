#include <vector>
#include <algorithm>
using namespace std;
#define ll long long
struct lll{
    vector<int> A;
    lll(ll x){
        if (x == 0){
            A.push_back(0);
        }
        else{
            while (x > 0){
                A.push_back(x % 10);
                x /= 10;
            }
        }
    }
    lll(string x){
        if (x == "0"){
            A.push_back(0);
        }
        else{
            while (x != ""){
                A.push_back(x.back() - '0');
                x.pop_back();
            }
        }
    }
    lll(vector<int> x){
        A = x;
    }
    void print(){
        string out = "";
        for (ll i = 0; i < A.size(); i++){
            out += to_string(A[i]);
        }
        reverse(out.begin(),out.end());
        cout << out << endl;
    }
    ll to_ll(){
        ll out = 0;
        ll t = 1;
        for (ll i = 0; i < A.size(); i++){
            out += A[i]*t;
            t *= 10;
        }
        return out;
    }
};

bool operator<(lll x, lll y){
    if (x.A.size() != y.A.size()){
        return x.A.size() < y.A.size();
    }
    else{
        for (ll i = x.A.size()-1; i >= 0; i--){
            if (x.A[i] < y.A[i]){
                return true;
            }
            if (x.A[i] > y.A[i]){
                return false;
            }
        }
        return false;
    }
}
bool operator>(lll x, lll y){
    if (x.A.size() != y.A.size()){
        return x.A.size() > y.A.size();
    }
    else{
        for (ll i = x.A.size()-1; i >= 0; i--){
            if (x.A[i] > y.A[i]){
                return true;
            }
            if (x.A[i] < y.A[i]){
                return false;
            }
        }
        return false;
    }
}
bool operator==(lll x, lll y){
    return x.A == y.A;
}
lll operator+(lll x, lll y){
    vector<int> R;
    for (ll i = 0; i < max(x.A.size(), y.A.size()); i++){
        R.push_back((i < x.A.size() ? x.A[i] : 0) + (i < y.A.size() ? y.A[i] : 0));
    }

    for (ll i = 0; i < R.size(); i++){
        if (R[i] >= 10){
            if (i == R.size()-1){R.push_back(0);}
            R[i+1] += R[i] / 10;
            R[i] %= 10;
        }
    }
    return lll(R);
}
lll operator-(lll x, lll y){
    vector<int> R;
    for (ll i = 0; i < max(x.A.size(), y.A.size()); i++){
        R.push_back((i < x.A.size() ? x.A[i] : 0) - (i < y.A.size() ? y.A[i] : 0));
    }

    for (ll i = 0; i < R.size(); i++){
        if (R[i] >= 10){
            if (i == R.size()-1){R.push_back(0);}
            R[i+1] += R[i] / 10;
            R[i] %= 10;
        }
        if (R[i] < 0){
            R[i+1] -= 1 + (abs(R[i])-1)/10;
            R[i] = (100+R[i])%10;
        }
    }
    for (ll i = R.size()-1; i >= 0; i--){
        if (R[i] == 0){
            R.pop_back();
        }
        else{
            break;
        }
    }
    return lll(R);
}
lll operator*(lll x, lll y){
    vector<int> R(x.A.size() + y.A.size() - 1,0);
    for (ll i = 0; i < x.A.size(); i++){
        for (ll j = 0; j < y.A.size(); j++){
            R[i+j] += x.A[i] * y.A[j];
        }
    }
    for (ll i = 0; i < R.size(); i++){//繰り上げ
        if (R[i] >= 10){
            if (i == R.size()-1){R.push_back(0);}
            R[i+1] += R[i] / 10;
            R[i] %= 10;
        }
    }
    for (ll i = R.size()-1; i >= 0; i--){//leading zeroを消す
        if (R[i] == 0){
            R.pop_back();
        }
        else{
            break;
        }
    }
    if (R.empty()){
        R.push_back(0);
    }
    return lll(R);
}
lll operator/(lll x, lll y){
    auto &digit_a = x.A; auto &digit_b = y.A;
    int NA = digit_a.size(), NB = digit_b.size();
	if(NA < NB) return {0};
	// ----- ステップ 1. A ÷ B の桁数を求める ----- //
	int D = NA - NB;
	// digit_a_partial : A の上 NB 桁を取り出したもの
	vector<int> digit_a_partial(digit_a.begin() + (NA - NB), digit_a.end());
	if(!(lll(digit_a_partial) < lll(digit_b))) {
		// (A の上 NB 桁) が B 以上だったら...？
		D = NA - NB + 1;
	}
	// ----- ステップ 2. A ÷ B を筆算で求める ----- //
	if(D == 0) return {0};
	vector<int> remain(digit_a.begin() + (D - 1), digit_a.end());
	vector<int> digit_ans(D);
	for(int i = D - 1; i >= 0; --i) {
		digit_ans[i] = 9;
		for(int j = 1; j <= 9; ++j) {
			vector<int> k = (lll(digit_b) * lll(vector<int>{j})).A;
			if(lll(k) > lll(remain)) {
				digit_ans[i] = j - 1;
				break;
			}
		}
		vector<int> x_result = (lll(digit_b) * vector<int>{ digit_ans[i] }).A;
		remain = (remain - x_result).A;
		if(i >= 1) {
			// 新しく 10^(i-1) の位が降りてくる
			remain.insert(remain.begin(), digit_a[i - 1]);
		}
	}
	return lll(digit_ans);
}

lll powlll(lll x, ll N){
    lll R = lll(1);
    for (ll i = 1; i <= N; i++){
        R = R * x;
    }
    return R;
}