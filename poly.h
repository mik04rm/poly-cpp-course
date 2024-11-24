#include <array>

template <typename T, std::size_t N = 0> class poly {
  private:
    std::array<T, N> coefs;

  public:
    constexpr std::size_t size() const { return N; }

    constexpr T& operator[](std::size_t i) const { return coefs[i]; }

    template <typename U, std::size_t M>
    constexpr poly& operator+=(const poly<U, M>& other) {
        std::size_t min_size = std::min(N, M);
        for (std::size_t i = 0; i < min_size; i++) {
            coefs[i] += other.coefs[i];
        }
        return *this;
    }
};