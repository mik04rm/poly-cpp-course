#include "poly.h"
#include <assert.h>

using namespace std;

template <typename T, std::size_t N>
constexpr bool operator==(const poly<T, N>& a, const poly<T, N>& b) {
    for (std::size_t i = 0; i < N; ++i)
        if (!(a[i] == b[i]))
            return false;
    return true;
}

constexpr poly<int, 2> compile_time_assignment() {
    poly r(1, 2);
    r = poly(3, 4); // Assignment at compile-time
    return r;
}

int main() {
    constexpr auto p = poly(2, 1);
    static_assert(p.size() == 2 && p[0] == 2 && p[1] == 1);
    static_assert(std::is_same_v<decltype(p), const poly<int, 2>>);

    constexpr auto q = poly(poly(1.0, 2.0), 3.0, 4.0);
    static_assert(std::is_same_v<decltype(q), const poly<poly<double, 2>, 3>>);
    static_assert(q[0][0] == 1.0);
    static_assert(q[0][1] == 2.0);
    static_assert(q[1][0] == 3.0);
    static_assert(q[1][1] == 0.0);
    static_assert(q[2][0] == 4.0);
    static_assert(q[2][1] == 0.0);

    static_assert(p + q == poly(poly(3.0, 2.0), 4.0, 4.0));
    static_assert(q + p == poly(poly(3.0, 2.0), 4.0, 4.0));
    static_assert(std::is_same_v<decltype(p + q), poly<poly<double, 2>, 3>>);
    static_assert(-q == poly(poly(-1.0, -2.0), -3.0, -4.0));
    static_assert(p - q == poly(poly(1.0, -2.0), -2.0, -4.0));
    static_assert(q - p == poly(poly(-1.0, 2.0), 2.0, 4.0));

    // constexpr auto xd = p * q;
    // constexpr auto dx = poly(poly(2.0, 4.0), poly(7.0, 2.0), 11.0, 4.0);

    static_assert(std::is_same_v<decltype(p * q), poly<poly<double, 2>, 4>>);
    static_assert(p * q == poly(poly(2.0, 4.0), poly(7.0, 2.0), 11.0, 4.0));
    static_assert(q * p == poly(poly(2.0, 4.0), poly(7.0, 2.0), 11.0, 4.0));

    constexpr auto pp(p);
    static_assert(p == pp);
    constexpr auto qq(q);
    static_assert(q == qq);

    constexpr auto result = compile_time_assignment();
    static_assert(result[0] == 3 && result[1] == 4);

    poly r(1, 2);
    poly rr(1, 2, 3, 4);
    r = poly(3);
    r = poly(1, 2);

    rr += r;
    assert(rr[0] == 2 && rr[1] == 4 && rr[2] == 3 && rr[3] == 4);

    constexpr auto x = poly(3, poly(1, 2), 4);
    static_assert(std::is_same_v<decltype(q), const poly<poly<double, 2>, 3>>);
    static_assert(x[0][0] == 3.0);
    static_assert(x[0][1] == 0.0);
    static_assert(x[1][0] == 1.0);
    static_assert(x[1][1] == 2.0);
    static_assert(x[2][0] == 4.0);
    static_assert(x[2][1] == 0.0);

    static_assert(q * 2.0 == poly(poly(2.0, 4.0), 6.0, 8.0));
    static_assert(2 * q == q * 2);

    constexpr auto s = poly(poly(poly(1, 2), 3), 4);
    static_assert(s * 2 == poly(poly(poly(2, 4), 6), 8));

    constexpr auto t = (poly<poly<int, 1>, 1>)2;
    static_assert(t[0][0] == 2);
    

}

// clang++ -Wall -Wextra -std=c++20 -O2 poly_example.cpp