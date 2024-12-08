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
    // constexpr auto q = poly(poly(1.0, 2.0), 3.0, 4.0);
    // constexpr auto res = q.at(2.0);
    // static_assert(std::is_same_v<decltype(res), const poly<double, 2>>);
    // constexpr auto res2 = q.at(q);
  

}

// clang++ -Wall -Wextra -std=c++20 -O2 poly_example3.cpp