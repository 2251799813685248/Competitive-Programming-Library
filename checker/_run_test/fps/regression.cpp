#include <fps_operations.hpp>
#include <random>
#include <vector>

// Deterministic references deliberately do not use NTT.
template<uint M> using Series = FormalPowerSeries<M>;
template<uint M> Series<M> naive(const Series<M>& a, const Series<M>& b, int limit = -1) {
    if (a.empty() || b.empty()) return Series<M>(0);
    int n = a.size()+b.size()-1;
    if (limit >= 0) n = std::min(n,limit);
    Series<M> c(n);
    for (int i=0; i<(int)a.size(); ++i)
        for (int j=0; j<(int)b.size() && i+j<n; ++j) c[i+j] += a[i]*b[j];
    return c;
}
template<uint M> void equal(const Series<M>& a, const Series<M>& b) {
    assert(a.size()==b.size());
    for (unsigned i=0;i<a.size();++i) assert(a[i]==b[i]);
}
template<uint M> void transforms_and_convolution() {
    using Mint = constant_modint<M>;
    fps_operator<M> op;
    std::mt19937 rng(123);
    for (int h=0; h<=13; ++h) {
        int n=1<<h;
        Series<M> a(n),b(n);
        for(int i=0;i<n;++i) {a[i]=rng()%M;b[i]=rng()%M;}
        auto fa=a,ib=b,ta=b,tb=a;
        op.inplaceDFT(fa); op.inplaceIDFT(ib);
        op.inplaceDFT_T(ta); op.inplaceIDFT_T(tb);
        Mint dot1=0,dot2=0,dot3=0,dot4=0;
        for(int i=0;i<n;++i) {
            assert(fa[i].val<M && ib[i].val<M && ta[i].val<M && tb[i].val<M);
            dot1+=fa[i]*b[i];dot2+=a[i]*ta[i];
            dot3+=a[i]*ib[i];dot4+=tb[i]*b[i];
        }
        assert(dot1==dot2 && dot3==dot4);
        if(n<=32) {
            Mint root=Mint(op.last_powroot).pow((1u<<op.log_max_length)/n);
            for(int j=0;j<n;++j) {
                int rev=0;
                for(int k=0;k<h;++k) rev=rev*2+((j>>k)&1);
                Mint w=root.pow(rev),v=0;
                for(int i=n-1;i>=0;--i) v=v*w+a[i];
                assert(v==fa[j]);
            }
        }
        op.inplaceIDFT(fa);
        for(int i=0;i<n;++i) assert(fa[i]==a[i]*Mint(n));
    }
    for(int n: {0,1,2,7,16,39,40,41,60,61,63,64,65,127,128,129,257}) {
        for(int m: {0,1,3,40,41,59,60,61,63,64,65,131}) {
            Series<M> a(n),b(m);
            for(int i=0;i<n;++i) a[i]=rng()%M;
            for(int i=0;i<m;++i) b[i]=rng()%M;
            equal(op.convolution(a,b),naive(a,b));
            for(int i=0;i<n;++i) a[i]=M-1;
            for(int i=0;i<m;++i) b[i]=M-1;
            equal(op.convolution(a,b),naive(a,b));
        }
    }
    // Lvalue/rvalue combinations, aliases and the overlap-add size boundary.
    for (auto [n,m]: {std::pair{1008,63}, {1007,63}, {1024,64}, {2049,65}, {4081,255}}) {
        Series<M> a(n),b(m);
        for (int i=0;i<n;++i) a[i]=rng()%M;
        for (int i=0;i<m;++i) b[i]=rng()%M;
        const auto expected=naive(a,b);
        equal(op.convolution(a,b),expected);
        equal(op.convolution(b,a),expected);
        auto x=a,y=b;
        equal(op.convolution(std::move(x),y),expected);
        x=a;
        equal(op.convolution(x,std::move(y)),expected);
        x=a;y=b;
        equal(op.convolution(std::move(x),std::move(y)),expected);
    }
    Series<M> same(65,M-1);
    const auto square=naive(same,same);
    auto x=same;
    equal(op.convolution(std::move(x),x),square);
    x=same;
    equal(op.convolution(x,std::move(x)),square);
    x=same;
    equal(op.convolution(std::move(x),std::move(x)),square);
    Series<M> empty;
    op.inplaceDFT(empty);op.inplaceIDFT(empty);assert(empty.empty());
}
int main() {
    transforms_and_convolution<998244353>();
    transforms_and_convolution<167772161>();
    transforms_and_convolution<469762049>();
    transforms_and_convolution<2013265921>(); // scalar fallback for a larger modulus
    // The original naive path also supports products longer than the NTT
    // limit of a small modulus when one operand has at most 60 terms.
    fps_operator<17> small_op;
    Series<17> small_a(60,16), small_b(60,15);
    equal(small_op.convolution(small_a,small_b),naive(small_a,small_b));
    constexpr uint M=998244353;
    using Mint=constant_modint<M>;
    fps_operator<M> op;
    mod_table<M> mt(4096);
    mod_table<M> boundary_table(32);
    Series<M> ones(32,1);
    equal(op.differential(op.integral(ones,boundary_table)),ones);
    assert(boundary_table.factorialmod(32)*boundary_table.factorialmodinv(32)==1);
    std::mt19937 rng(42);
    Series<M> empty;
    assert(op.inv(empty,0).empty());
    assert(op.exp(empty,0,mt).empty());
    assert(op.log(empty,0,mt).empty());
    assert(op.pow(empty,0,0,mt).empty());
    auto huge_power = op.pow(Series<M>{0,1},~0ull,17,mt);
    for (int i=0;i<17;++i) assert(huge_power[i]==0);
    huge_power = op.pow(Series<M>{2},~0ull,17,mt);
    assert(huge_power[0]==Mint(2).pow(~0ull));
    for (int i=1;i<17;++i) assert(huge_power[i]==0);
    auto exp_zero=op.exp(empty,17,mt);
    assert(exp_zero[0]==1);
    for(int i=1;i<17;++i) assert(exp_zero[i]==0);
    for(int n: {1,2,3,7,16,31,32,33,65,129,257}) {
        Series<M> f(n);
        for(int i=0;i<n;++i) f[i]=rng()%M;
        f[0]=1;
        auto inv=op.inv(f,n), unit=naive(f,inv,n);
        assert(unit[0]==1);
        for(int i=1;i<n;++i) assert(unit[i]==0);
        equal(op.exp(op.log(f,n,mt),n,mt),f);
        auto df=op.differential(f), intf=op.integral(df,mt);
        intf.resize(n);intf[0]=1;equal(intf,f);
        for(int shift: {0,1,n/2}) {
            auto g=f;
            for(int i=0;i<shift;++i) g[i]=0;
            Series<M> acc(n);acc[0]=1;
            for(int k=0;k<=5;++k) {
                equal(op.pow(g,k,n,mt),acc);
                acc=naive(acc,g,n);acc.resize(n);
            }
        }
        Series<M> b(std::max(1,n/2));
        for(unsigned i=0;i<b.size();++i) b[i]=rng()%M;
        b[b.size()-1]=1;
        auto qr=op.div(f,b);
        auto reconstruct=op.add(naive(qr[0],b),qr[1]);
        reconstruct.resize(n);equal(reconstruct,f);
        equal(op.remainder(f,b),qr[1]);
        if(n<=65) {
            auto shifted=op.polynominal_taylor_shift(f,7,mt);
            Series<M> expected(n), power{1};
            for(int i=0;i<n;++i) {
                for(unsigned j=0;j<power.size();++j) expected[j]+=f[i]*power[j];
                power=naive(power,Series<M>{7,1},n);
            }
            equal(shifted,expected);
            Series<M> g(n);g[0]=0;
            for(int i=1;i<n;++i) g[i]=rng()%M;
            expected=Series<M>(n);power=Series<M>{1};
            for(int i=0;i<n;++i) {
                for(unsigned j=0;j<power.size();++j) expected[j]+=f[i]*power[j];
                power=naive(power,g,n);
            }
            equal(op.composition(f,g,mt),expected);
            if(n>=2) {
                auto gi=op.compositional_inverse(g,mt), id=op.composition(g,gi,mt);
                assert(id[0]==0 && id[1]==1);
                for(int i=2;i<n;++i) assert(id[i]==0);
            }
        }
    }
    std::cout << "FPS regression passed\n";
}
