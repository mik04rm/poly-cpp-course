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
    poly x(p);

}

// clang++ -Wall -Wextra -std=c++20 -O2 poly_example2.cpp