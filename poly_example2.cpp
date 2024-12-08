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
    auto p = poly(2, 1);
     std::cout << "-----------------------------------" << std::endl;
    auto q = poly(p, p, p);
     std::cout << "-----------------------------------" << std::endl;
    auto r = poly(poly(2, 1), poly(2, 1), poly(2, 1));
    // poly x(p);

    std::common_type_t<poly<int,2>, poly<int,2>> a;
    auto y = poly(poly(1.0, 2.0), 3.0, 4.0);
    auto x = poly(2 * y, y);

    constexpr auto z = poly(poly(1.0, 2.0), 3.0, 4.0);

}

// clang++ -Wall -Wextra -std=c++20 -O2 poly_example2.cpp