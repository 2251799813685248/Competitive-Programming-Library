#include <fps_operations.hpp>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>

// Only convolution (including by-value copies, allocation and normalization)
// is timed. Input generation, checksum and all I/O are outside the timer.
int main(int argc, char** argv) {
    const int n=argc>1 ? std::atoi(argv[1]) : 524288;
    const int m=argc>2 ? std::atoi(argv[2]) : n;
    const int reps=argc>3 ? std::atoi(argv[3]) : 21;
    if(n<1 || m<1 || reps<1) return 1;
    fps_operator<998244353> op;
    FormalPowerSeries<998244353> a(n),b(m);
    std::mt19937 rng(123);
    for(int i=0;i<n;++i) a[i]=rng()%998244353;
    for(int i=0;i<m;++i) b[i]=rng()%998244353;
    std::vector<double> times;
    unsigned long long checksum=0;
    for(int r=0;r<reps;++r) {
        const auto start=std::chrono::steady_clock::now();
        const auto c=op.convolution(a,b);
        const auto end=std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration<double,std::milli>(end-start).count());
        for(unsigned i=0;i<c.size();++i) checksum+=c[i].val;
    }
    const double first=times.front();
    std::sort(times.begin(),times.end());
    std::cout << std::fixed << std::setprecision(3)
              << "n="<<n<<" m="<<m<<" reps="<<reps
              << " first_ms="<<first<<" median_ms="<<times[times.size()/2]
              << " min_ms="<<times.front()<<" checksum="<<checksum<<'\n';
}
