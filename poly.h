#include <array>

template <typename T, std::size_t N = 0> class poly {
    std::array<T, N> coefs;
};