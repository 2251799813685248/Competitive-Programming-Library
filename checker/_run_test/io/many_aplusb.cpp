#include <fastio.hpp>
int main() {
    int t;
    if (!fastio::read(t)) return 1;
    while (t--) {
#ifdef FASTIO_INT128
        __int128_t a,b;
#else
        long long a,b;
#endif
        if (!fastio::read(a,b) || !fastio::print(a+b)) return 1;
    }
    return fastio::flush() ? 0 : 1;
}
