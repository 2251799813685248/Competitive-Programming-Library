#include <fastio.hpp>
#include <fps.hpp>
#include <cassert>
#include <cmath>
#include <limits>
#include <random>
#include <vector>

static FILE* input_file(std::string_view text) {
    FILE* f = std::tmpfile();
    assert(f);
    assert(std::fwrite(text.data(),1,text.size(),f)==text.size());
    std::rewind(f);
    return f;
}
static std::string contents(FILE* f) {
    std::rewind(f);
    std::string result;
    char buf[4096];
    while (const auto n=std::fread(buf,1,sizeof(buf),f)) result.append(buf,n);
    return result;
}

template<std::size_t B> void boundary_tests() {
    FILE* f=input_file(" \r\n\t-9223372036854775808 +9223372036854775807 18446744073709551615 -0 0 1  -128 255\nhello X +1.25 -2.5e-3 -998244354 18446744073709551615 -101 1 2 3 4 5 6 7 8 9");
    {
        fastio::BasicInput<B> in(f);
        long long lo=0,hi=0;
        unsigned long long umax=0;
        int zero=1;
        bool b0=true,b1=false;
        signed char small=0;
        unsigned char large=0;
        assert(in.read(lo,hi,umax,zero,b0,b1,small,large));
        assert(lo==std::numeric_limits<long long>::min());
        assert(hi==std::numeric_limits<long long>::max());
        assert(umax==std::numeric_limits<unsigned long long>::max());
        assert(zero==0 && !b0 && b1 && small==-128 && large==255);
        std::string word;
        char ch=0;
        double x=0,y=0;
        assert(in.read(word,ch,x,y));
        assert(word=="hello" && ch=='X' && x==1.25 && y==-0.0025);
        constant_modint<998244353> m1,m2;
        dynamic_modint<101>::set_mod(97);
        dynamic_modint<101> m3;
        assert(in.read(m1,m2,m3));
        assert(m1.val==998244352 && m2.val==umax%998244353 && m3.val==93);
        std::vector<int> v(2);
        list<int> ls(2);
        FormalPowerSeries<998244353> fps(2);
        std::pair<int,int> pair;
        std::tuple<int> tuple;
        assert(in.read(v,ls,fps,pair,tuple));
        assert(v[0]==1 && v[1]==2 && ls[0]==3 && ls[1]==4);
        assert(fps[0]==5 && fps[1]==6 && pair.first==7 && pair.second==8 && std::get<0>(tuple)==9);
        assert(static_cast<bool>(in)); // last token ends exactly at EOF
        int unchanged=71;
        assert(!in.read(unchanged) && unchanged==71 && !in);
    }
    std::fclose(f);
    f=std::tmpfile(); assert(f);
    {
        fastio::BasicOutput<B> out(f);
        assert(out.print(std::numeric_limits<long long>::min(),std::numeric_limits<long long>::max(),std::numeric_limits<unsigned long long>::max()));
        assert(out.print(false,true,static_cast<signed char>(-128),static_cast<unsigned char>(255)));
        assert(out.write("hello",' ',std::string("world"),std::string_view("!"),'\n'));
        assert(out.print(std::vector<int>{1,2},std::array<int,2>{3,4},std::pair<int,int>{5,6},std::tuple<int,int>{7,8}));
        list<int> ls{9,10};
        FormalPowerSeries<998244353> fps{11,12};
        assert(out.print(ls,fps,constant_modint<998244353>(-1)));
        assert(out.print(std::vector<int>{}));
        assert(out.print());
        out << 13 << ' ' << -14 << '\n';
        assert(out.flush() && static_cast<bool>(out));
        assert(out.print(15)); // destructor flush
    }
    assert(contents(f)=="-9223372036854775808 9223372036854775807 18446744073709551615\n0 1 -128 255\nhello world!\n1 2 3 4 5 6 7 8\n9 10 11 12 998244352\n\n\n13 -14\n15\n");
    std::fclose(f);
}

void int128_tests() {
#if defined(__SIZEOF_INT128__)
    FILE* f=input_file("-170141183460469231731687303715884105728 170141183460469231731687303715884105727 340282366920938463463374607431768211455");
    __int128_t lo,hi;
    __uint128_t u;
    {
        fastio::BasicInput<3> in(f);
        assert(in.read(lo,hi,u));
        assert(u==~__uint128_t(0));
        assert(hi==static_cast<__int128_t>(u>>1));
        assert(lo==-hi-1);
    }
    std::fclose(f);
    f=std::tmpfile(); assert(f);
    {
        fastio::BasicOutput<7> out(f);
        assert(out.print(lo,hi,u));
    }
    assert(contents(f)=="-170141183460469231731687303715884105728 170141183460469231731687303715884105727 340282366920938463463374607431768211455\n");
    std::fclose(f);
#endif
}

void random_roundtrip() {
    std::mt19937_64 gen(42);
    std::vector<unsigned long long> u(50000);
    std::vector<long long> s(u.size());
    for(std::size_t i=0;i<u.size();++i) {
        u[i]=gen();
        s[i]=static_cast<long long>(gen()>>1);
        if(i&1) s[i]=-s[i];
    }
    FILE* f=std::tmpfile(); assert(f);
    {
        fastio::FastOutput out(f);
        assert(out.print(u)); assert(out.print(s));
    }
    std::rewind(f);
    {
        fastio::FastInput in(f);
        std::vector<unsigned long long> u2(u.size());
        std::vector<long long> s2(s.size());
        assert(in.read(u2,s2));
        assert(u==u2 && s==s2);
    }
    std::fclose(f);
    // Long strings exercise multiple refills and output drains.
    const std::string text(200000,'z');
    f=std::tmpfile(); assert(f);
    {
        fastio::BasicOutput<31> out(f);
        assert(out.print(text,1.25,-0.0025));
    }
    std::rewind(f);
    {
        fastio::BasicInput<17> in(f);
        std::string got; double a,b;
        assert(in.read(got,a,b));
        assert(got==text && a==1.25 && b==-0.0025);
    }
    std::fclose(f);
}

template<std::size_t B> void decimal_boundaries() {
    std::vector<unsigned> u{0,9,10,99,100,999,1000,9999,10000,99999999,100000000,999999999,1000000000,4294967295u};
    std::vector<int> v{0,-1,-9,-10,-99,-100,-999,-1000,-9999,-10000,2147483647,-2147483647-1};
    std::mt19937 rng(72);
    for (int i=0;i<10000;++i) u.push_back(rng());
    std::string expected;
    for (unsigned x:u) { if (!expected.empty()) expected+=' '; expected+=std::to_string(x); }
    expected+='\n';
    for (std::size_t i=0;i<v.size();++i) { if (i) expected+=' '; expected+=std::to_string(v[i]); }
    expected+='\n';
    FILE* f=std::tmpfile(); assert(f);
    { fastio::BasicOutput<B> out(f); assert(out.print(u)); assert(out.print(v)); }
    assert(contents(f)==expected);
    std::rewind(f);
    {
        fastio::BasicInput<B> in(f);
        std::vector<unsigned> got(u.size()); std::vector<int> signed_got(v.size());
        assert(in.read(got,signed_got)); assert(got==u && signed_got==v);
    }
    std::fclose(f);
    // Long leading zeroes, an 8-digit token ending at a refill, and no final LF.
    f=input_file(std::string(97,'0')+"12345678\t-0000000012345678 99999999");
    {
        fastio::BasicInput<B> in(f);
        int a,b,c; assert(in.read(a,b,c));
        assert(a==12345678 && b==-12345678 && c==99999999);
    }
    std::fclose(f);
}

int main() {
    decimal_boundaries<8>(); decimal_boundaries<9>(); decimal_boundaries<17>();
    decimal_boundaries<32>(); decimal_boundaries<64>(); decimal_boundaries<65536>();
    boundary_tests<1>(); boundary_tests<2>(); boundary_tests<3>();
    boundary_tests<7>(); boundary_tests<64>(); boundary_tests<65536>();
    int128_tests(); random_roundtrip();
    for(const std::string text: {"", " \r\n\t", "-", "+", "hello"}) {
        FILE* f=input_file(text);
        { fastio::BasicInput<1> in(f); int x=42; assert(!in.read(x) && x==42 && !in); }
        std::fclose(f);
    }
    FILE* f=input_file("-1");
    { fastio::FastInput in(f); unsigned x=42; assert(!in.read(x) && x==42); }
    std::fclose(f);
    // Linux error propagation, without relying on destructor return values.
    f=std::fopen("/dev/full","wb");
    if(f) {
        { fastio::FastOutput out(f); out.print(123); assert(!out.flush() && !out); }
        std::fclose(f);
    }
    std::puts("fastio regression passed");
}
