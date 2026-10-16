#include <fastio.hpp>
#include <fps_operations.hpp>
#ifdef FASTIO_PROFILE
#include <chrono>
#endif
fps_operator<998244353> op;
int main() {
#ifdef FASTIO_USE_IOSTREAM
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
#endif
#ifdef FASTIO_PROFILE
    using Clock = std::chrono::steady_clock;
    const auto t0 = Clock::now();
#endif
    int n,m;
#ifdef FASTIO_USE_IOSTREAM
    if (!(std::cin >> n >> m)) return 1;
#else
    if (!fastio::read(n,m)) return 1;
#endif
    FormalPowerSeries<998244353> a(n),b(m);
#ifdef FASTIO_PROFILE
    const auto t1 = Clock::now();
#endif
#ifdef FASTIO_USE_IOSTREAM
    for (int i=0;i<n;++i) std::cin >> a[i];
    for (int i=0;i<m;++i) std::cin >> b[i];
    if (!std::cin) return 1;
#else
    if (!fastio::read(a,b)) return 1;
#endif
#ifdef FASTIO_PROFILE
    const auto t2 = Clock::now();
#endif
    auto c = op.convolution(a,b);
#ifdef FASTIO_PROFILE
    const auto t3 = Clock::now();
#endif
#ifdef FASTIO_USE_IOSTREAM
    for (uint i=0;i<c.size();++i) {
        if (i) std::cout << ' ';
        std::cout << c[i];
    }
    std::cout << '\n';
    std::cout.flush();
    if (!std::cout) return 1;
#else
    if (!fastio::print(c) || !fastio::flush()) return 1;
#endif
#ifdef FASTIO_PROFILE
    const auto t4 = Clock::now();
    auto ms=[](auto a,auto b) { return std::chrono::duration<double,std::milli>(b-a).count(); };
    std::fprintf(stderr,"%.3f %.3f %.3f %.3f %.3f\n",ms(t0,t1),ms(t1,t2),ms(t2,t3),ms(t3,t4),ms(t0,t4));
#endif
}
