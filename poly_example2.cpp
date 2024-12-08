#include "poly.h"
#include <assert.h>


template <typename T, std::size_t N>
  constexpr bool operator==(const poly<T, N>& a, const poly<T, N>& b)
  {
    for (std::size_t i = 0; i < N; ++i)
      if (!(a[i] == b[i]))
        return false;
    return true;
  }

int main() {
    constexpr auto p = poly(2, 1);
    std::cout << "-----------------------------------" << std::endl;
    constexpr auto q = poly(p, p, p);
    static_assert(std::is_same_v<decltype(q), const poly<poly<int, 2>, 3>>);
    static_assert(q == poly(poly(2, 1), poly(2, 1), poly(2, 1)));
    std::cout << "-----------------------------------" << std::endl;
    constexpr auto r = poly(poly(2, 1), poly(2, 1), poly(2, 1));

    static_assert(r == q);

    constexpr auto y = poly(poly(1.0, 2.0), 3.0, 4.0);
    constexpr auto x = poly(2 * y, y);
    static_assert(x == poly(poly(poly(2.0, 4.0), 6.0, 8.0), poly(poly(1.0, 2.0), 3.0, 4.0)));

    constexpr auto w = const_poly(2 * y);
    static_assert(w[0] == poly(poly(2.0, 4.0), 6.0, 8.0));
    static_assert(w.size() == 1);
  
    constexpr auto v = 2.5 * p;
    static_assert(v == poly(5, 2.5));


}

// clang++ -Wall -Wextra -std=c++20 -O2 poly_example2.cpp