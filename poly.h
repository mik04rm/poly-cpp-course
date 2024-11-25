#pragma once

#include <array>
#include <type_traits>
#include <utility>

namespace detail {
    // Helper to determine the common type for poly
    template <typename T, typename U>
    struct poly_common_type;

    template <typename T, std::size_t N, typename U, std::size_t M>
    struct poly_common_type<struct poly<T, N>, struct poly<U, M>> {
        using type = struct poly<std::common_type_t<T, U>, (N > M ? N : M)>;
    };

    template <typename T, std::size_t N, typename U>
    struct poly_common_type<struct poly<T, N>, U> {
        using type = struct poly<std::common_type_t<T, U>, N>;
    };

    template <typename T, typename U, std::size_t M>
    struct poly_common_type<T, struct poly<U, M>> {
        using type = struct poly<std::common_type_t<T, U>, M>;
    };
}

template <typename T, std::size_t N = 0>
class poly {
public:
    std::array<T, N> coefs;

    constexpr poly() : coefs{} {}

    template <typename U, std::size_t M, typename = std::enable_if_t<(M <= N) && std::is_convertible_v<U, T>>>
    constexpr poly(const poly<U, M>& other) {
        for (std::size_t i = 0; i < M; ++i) {
            coefs[i] = other.coefs[i];
        }
        for (std::size_t i = M; i < N; ++i) {
            coefs[i] = T{};
        }
    }

    template <typename U, std::size_t M, typename = std::enable_if_t<(M <= N) && std::is_convertible_v<U, T>>>
    constexpr poly(poly<U, M>&& other) {
        for (std::size_t i = 0; i < M; ++i) {
            coefs[i] = std::move(other.coefs[i]);
        }
        for (std::size_t i = M; i < N; ++i) {
            coefs[i] = T{};
        }
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    constexpr poly(U&& value) {
        static_assert(N == 1, "Converting constructor can only be used when N == 1");
        coefs[0] = std::forward<U>(value);
    }

    template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) <= N)>>
    constexpr poly(Args&&... args) : coefs{std::forward<Args>(args)...} {}

    template <typename U, std::size_t M, typename = std::enable_if_t<(M <= N) && std::is_convertible_v<U, T>>>
    constexpr poly& operator=(const poly<U, M>& other) {
        for (std::size_t i = 0; i < M; ++i) {
            coefs[i] = other.coefs[i];
        }
        for (std::size_t i = M; i < N; ++i) {
            coefs[i] = T{};
        }
        return *this;
    }

    template <typename U, std::size_t M, typename = std::enable_if_t<(M <= N) && std::is_convertible_v<U, T>>>
    constexpr poly& operator=(poly<U, M>&& other) {
        for (std::size_t i = 0; i < M; ++i) {
            coefs[i] = std::move(other.coefs[i]);
        }
        for (std::size_t i = M; i < N; ++i) {
            coefs[i] = T{};
        }
        return *this;
    }

    constexpr T& operator[](std::size_t i) {
        return coefs[i];
    }

    constexpr const T& operator[](std::size_t i) const {
        return coefs[i];
    }

    template <typename... Args>
    constexpr auto at(Args&&... args) const {
        return evaluate(*this, std::forward<Args>(args)...);
    }

    template <typename U, std::size_t K>
    constexpr auto at(const std::array<U, K>& arr) const {
        return at_array(*this, arr);
    }

    constexpr std::size_t size() const {
        return N;
    }

    template <typename U, std::size_t M>
    constexpr poly& operator+=(const poly<U, M>& other) {
        constexpr std::size_t min_size = (N < M) ? N : M;
        for (std::size_t i = 0; i < min_size; ++i) {
            coefs[i] += other.coefs[i];
        }
        return *this;
    }

    template <typename U>
    constexpr poly& operator+=(const U& value) {
        static_assert(N >= 1, "Polynomial size must be at least 1 to add a scalar");
        coefs[0] += value;
        return *this;
    }

    template <typename U, std::size_t M>
    constexpr poly& operator-=(const poly<U, M>& other) {
        constexpr std::size_t min_size = (N < M) ? N : M;
        for (std::size_t i = 0; i < min_size; ++i) {
            coefs[i] -= other.coefs[i];
        }
        return *this;
    }

    template <typename U>
    constexpr poly& operator-=(const U& value) {
        static_assert(N >= 1, "Polynomial size must be at least 1 to subtract a scalar");
        coefs[0] -= value;
        return *this;
    }

    template <typename U>
    constexpr poly& operator*=(const U& value) {
        for (std::size_t i = 0; i < N; ++i) {
            coefs[i] *= value;
        }
        return *this;
    }

private:
    template <typename P, typename Arg>
    static constexpr auto evaluate(const P& p, Arg&& arg) {
        using ResultType = decltype(p.coefs[0] * arg);
        ResultType result = ResultType{};
        for (std::size_t i = 0; i < N; ++i) {
            ResultType term = p.coefs[i];
            for (std::size_t j = 0; j < i; ++j) {
                term *= arg;
            }
            result += term;
        }
        return result;
    }

    template <typename P, typename Arg, typename... Args>
    static constexpr auto evaluate(const P& p, Arg&& arg, Args&&... args) {
        using CoefType = typename P::value_type;
        CoefType result = CoefType{};
        for (std::size_t i = 0; i < N; ++i) {
            CoefType term = p.coefs[i].at(std::forward<Args>(args)...);
            for (std::size_t j = 0; j < i; ++j) {
                term *= arg;
            }
            result += term;
        }
        return result;
    }

    template <typename P, typename U, std::size_t K>
    static constexpr auto at_array(const P& p, const std::array<U, K>& arr) {
        return at_array_impl(p, arr, std::make_index_sequence<K>{});
    }

    template <typename P, typename U, std::size_t K, std::size_t... Is>
    static constexpr auto at_array_impl(const P& p, const std::array<U, K>& arr, std::index_sequence<Is...>) {
        return p.at(arr[Is]...);
    }
};

// Deduction guides
template <typename... Args>
poly(Args&&...) -> poly<std::common_type_t<Args...>, sizeof...(Args)>;

template <typename T, std::size_t N>
constexpr auto const_poly(const poly<T, N>& p) {
    return poly<poly<T, N>, 1>{p};
}

template <typename T, std::size_t N>
constexpr auto operator-(const poly<T, N>& p) {
    poly<T, N> result;
    for (std::size_t i = 0; i < N; ++i) {
        result.coefs[i] = -p.coefs[i];
    }
    return result;
}

template <typename LHS, typename RHS>
constexpr auto operator+(const LHS& lhs, const RHS& rhs) {
    using CommonType = std::common_type_t<LHS, RHS>;
    return CommonType(lhs) += rhs;
}

template <typename LHS, typename RHS>
constexpr auto operator-(const LHS& lhs, const RHS& rhs) {
    using CommonType = std::common_type_t<LHS, RHS>;
    return CommonType(lhs) -= rhs;
}

template <typename LHS, typename RHS>
constexpr auto operator*(const LHS& lhs, const RHS& rhs) {
    using CommonType = std::common_type_t<LHS, RHS>;
    return CommonType(lhs) *= rhs;
}

// Specialization of std::common_type
namespace std {
    template <typename T, std::size_t N, typename U, std::size_t M>
    struct common_type<poly<T, N>, poly<U, M>> {
        using type = poly<std::common_type_t<T, U>, (N > M ? N : M)>;
    };

    template <typename T, std::size_t N, typename U>
    struct common_type<poly<T, N>, U> {
        using type = poly<std::common_type_t<T, U>, N>;
    };

    template <typename T, typename U, std::size_t M>
    struct common_type<T, poly<U, M>> {
        using type = poly<std::common_type_t<T, U>, M>;
    };
}

template <typename P, typename Q>
constexpr auto cross(const P& p, const Q& q) {
    using ResultType = poly<P, 1>;
    return ResultType{p} * poly<Q, 1>{q};
}