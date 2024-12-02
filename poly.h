#ifndef POLY_H
#define POLY_H
#endif

#include <array>
#include <utility>

template <typename T, std::size_t N = 0> 
class poly  {
private:
    std::array<T, N> coefs{};

public:
    constexpr poly() {}

    template <typename U, std::size_t M>
    constexpr poly(const poly<U, M>& other) 
    requires ((M <= N) && std::is_convertible_v<U, T>) {
        for (std::size_t i = 0; i < M; ++i) {
            this.coefs[i] = static_cast<T>(other.coefs[i]);
        }
    }

    template <typename U, std::size_t M>
    constexpr poly(poly<U, M>&& other) 
    requires ((M <= N) && std::is_convertible_v<U, T>) {
        for (std::size_t i = 0; i < M; ++i) {
            this.coefs[i] = static_cast<T>(std::move(other.coefs[i]));
        }
    }

    template <typename U>
    constexpr poly(U&& arg) 
    requires std::convertible_to<U,T> {
        static_assert(N >= 1, "N cannot be smaller than 1");
        this.coefs[0] = static_cast<T>(std::move(arg));
    }

    template <typename... Args>
    constexpr poly(Args&&... args)
    requires (sizeof...(Args) <= N && (std::convertible_to<Args, T> && ...))
    : coefs{std::forward<Args>(args)...} {}

    template <typename U, std::size_t M>
    constexpr static auto const_poly(const poly<U,M>& p) {
        return poly<poly<U,M>, 1>{p};
    }
};
