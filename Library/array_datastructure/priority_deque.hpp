#ifndef PRIORITY__deque_HPP_
#define PRIORITY__deque_HPP_


#include <vector>
#include <cmath>
#include <algorithm>
#include <cassert>
using namespace std;


/// @brief 両端優先度付きキュー
/// @tparam T 
template<typename T>
struct priority_deque{
    private:
    vector<T> A;
    void adjust_max_heap_from_bottom(int idx){
        while (idx > 0){
            if (A[((idx-1)>>1)&-2] < A[idx]){
                swap(A[((idx-1)>>1)&-2], A[idx]);
            }
            else{return;}
            idx--;
            idx>>=1;
            idx&=-2;
        }
    }
    void adjust_min_heap_from_bottom(int idx){
        while (idx > 1){
            if (A[idx] < A[((idx-2)>>1)|1]){
                swap(A[idx], A[((idx-2)>>1)|1]);
            }
            else{return;}
            idx-=2;
            idx>>=1;
            idx|=1;
        }
    }
    void build_heap(){
        for (int i = A.size()-1; i >= 0; i--){
            if ((i&1) && A[i-1] < A[i]){swap(A[i-1], A[i]);}
            
            int n = A.size();
            int t = i;
	        if (t&1){
	            while (2*t+1 < n){
	        	int c = 2*t+3;
	        	if (n <= c || A[c-2] < A[c]) c -= 2;
	        	if (c < n && A[c] < A[t]){swap(A[t], A[c]); t = c;}
	        	else break;
	            }
	        } else {
	            while (2*t+2 < n){
	        	int c = 2*t+4;
	        	if (n <= c || A[c] < A[c-2]) c -= 2;
	        	if (c < n && A[t] < A[c]){swap(A[t], A[c]); t = c;}
	        	else break;
	            }
	        }
            if ((t|1) < (int)A.size() && A[t&-2] < A[t|1]){
	            swap(A[t&-2], A[t|1]);
	            t ^= 1;
	        }
        
	        int p;
	        while (i < t && A[p=(((t>>1)-1)&-2)] < A[t]){
	            swap(A[p], A[t]);
	            t = p;
	        }
	        while (i < t && A[t] < A[p=(((t>>1)-1)|1)]){
	            swap(A[p], A[t]);
	            t = p;
	        }
        }
    }
    public:
    priority_deque(const vector<T>& B){
        A = B;
        build_heap();
    }
    priority_deque(){}
    void push(const T& value){
        if (A.size()&1){
            if (A.back() < value){
                A.insert(prev(A.end()), value);
                adjust_max_heap_from_bottom(A.size()-2);
            }
            else{
                A.push_back(value);
                adjust_min_heap_from_bottom(A.size()-1);
            }
        }
        else{
            A.push_back(value);
            if (A.size() == 1){return;}
            if (A[((A.size()-2)>>1)&-2] < A.back()){
                adjust_max_heap_from_bottom(A.size()-1);
            }
            else if (A.back() < A[((A.size()-2)>>1)|1]){
                swap(A.back(), A[((A.size()-2)>>1)|1]);
                adjust_min_heap_from_bottom(((A.size()-2)>>1)|1);
            }
        }
    }
    T pop_min(){
        if (A.size() == 1){T temp = std::move(A[0]); A.pop_back(); return temp;}
        T ret = std::move(A[1]);
        A[1] = A.back();
        A.pop_back();
        int idx = 1;
        while ((idx<<1)+1 < (int)A.size()){
            if ((idx<<1)+3 < (int)A.size()){
                if (A[idx] < A[(idx<<1)+1] && A[idx] < A[(idx<<1)+3]){
                    return ret;
                }
                if (A[(idx<<1)+1] < A[(idx<<1)+3]){
                    swap(A[idx], A[(idx<<1)+1]);
                    idx<<=1;
                    idx+=1;
                }
                else{
                    swap(A[idx], A[(idx<<1)+3]);
                    idx<<=1;
                    idx+=3;
                }
            }
            else{
                if (A[(idx<<1)+1] < A[idx]){
                    swap(A[idx], A[(idx<<1)+1]);
                }
                idx<<=1;
                idx+=1;
                break;
            }
        }
        if (A.size() < 2){return ret;}
        if (A[idx-1] < A[idx]){
            swap(A[idx], A[idx-1]);
            adjust_max_heap_from_bottom(idx-1);
        }
        return ret;
    }
    T pop_max(){
        T ret = std::move(A[0]);
        A[0] = A.back();
        A.pop_back();
        int idx = 0;
        while ((idx<<1)+2 < (int)A.size()){
            if ((idx<<1)+4 < (int)A.size()){
                if (A[(idx<<1)+2] < A[idx] && A[(idx<<1)+4] < A[idx]){
                    return ret;
                }
                if (A[(idx<<1)+2] < A[(idx<<1)+4]){
                    swap(A[idx], A[(idx<<1)+4]);
                    idx<<=1;
                    idx+=4;
                }
                else{
                    swap(A[idx], A[(idx<<1)+2]);
                    idx<<=1;
                    idx+=2;
                }
            }
            else{
                if (A[idx] < A[(idx<<1)+2]){
                    swap(A[idx], A[(idx<<1)+2]);
                }
                idx<<=1;
                idx+=2;
                break;
            }
        }
        if (A.size() < 2){return ret;}
        if (idx+1 == (int)A.size()){
            if (A.back() < A[((idx-1)>>1)|1]){
                swap(A.back(), A[((idx-1)>>1)|1]);
                adjust_min_heap_from_bottom(((idx-1)>>1)|1);
            }
        }
        else{
            if (A[idx] < A[idx+1]){
                swap(A[idx], A[idx+1]);
                adjust_min_heap_from_bottom(idx+1);
            }
        }
        return ret;
    }
    T get_min(){return A.size() > 1 ? A[1] : A[0];}
    T get_max(){return A[0];}
    bool empty(){return A.empty();}
    size_t size(){return A.size();}
};



#endif /* PRIORITY__deque_HPP_ */