#ifndef POLY_H
#define POLY_H
#endif

#include <array>
#include <utility>
#include <iostream>

#define DEBUG 1

#if DEBUG
    #define DBG_PRINT(x)                                           \
        if (!std::is_constant_evaluated()) {                       \
            std::cout << x << std::endl;                          \
        }
#else
    #define DBG_PRINT(x)
#endif

template <typename U, typename T, std::size_t M, std::size_t N>
concept PolyConvertible = (M <= N) && std::is_convertible_v<U, T>;

template <typename T, typename U>
concept ConvertibleTo = requires { 
    std::common_type<T, U>::type; 
};

template <typename T, std::size_t N = 0> 
class poly  {

private:
    std::array<T, N> coefs{};

public:
    
    constexpr poly() {
        DBG_PRINT("empty constructor called");
    }

    template <typename U, std::size_t M>
    constexpr poly(const poly<U, M>& other) 
    requires PolyConvertible<U, T, M, N> {
        DBG_PRINT("copy constructor called");
        for (std::size_t i = 0; i < M; i++) {
            this->coefs[i] = static_cast<T>(other.coefs[i]);
        }
    }

    template <typename U, std::size_t M>
    constexpr poly(poly<U, M>&& other) 
    requires PolyConvertible<U, T, M, N> {
        DBG_PRINT("move constructor called");
        for (std::size_t i = 0; i < M; i++) {
            this->coefs[i] = static_cast<T>(std::move(other.coefs[i]));
        }
    }

    template <typename U>
    constexpr poly(U&& arg) 
    requires std::convertible_to<U, T> {
        DBG_PRINT("single constructor called");
        static_assert(N >= 1, "N cannot be smaller than 1");
        this->coefs[0] = static_cast<T>(std::move(arg));
    }

    template <typename... Args>
    constexpr poly(Args&&... args)
    requires (sizeof...(Args) <= N && (std::convertible_to<Args, T> && ...))
    : coefs{std::forward<Args>(args)...} {
        DBG_PRINT("variadic constructor called");
    }

    template <typename U, std::size_t M>
    constexpr static auto const_poly(const poly<U,M>& p) {
        DBG_PRINT("const_poly constructor called");
        return poly<poly<U,M>, 1>{p};
    }
    
    constexpr std::size_t size() const {
        return N;
    }
    
    constexpr T& operator[](std::size_t i) {
        return coefs[i];
    }

    constexpr const T& operator[](std::size_t i) const {
        return coefs[i];
    }

    // Copy assignment operator
    template <typename U, std::size_t M>
    constexpr poly& operator=(const poly<U, M>& other)
    requires PolyConvertible<U, T, M, N> {
        for (std::size_t i = 0; i < M; i++) {
            coefs[i] = static_cast<T>(other.coefs[i]);
        }
        return *this;
    }

    // Move assignment operator
    template <typename U, std::size_t M>
    constexpr poly& operator=(poly<U, M>&& other)
    requires PolyConvertible<U, T, M, N> {
        for (std::size_t i = 0; i < M; i++) {
            coefs[i] = static_cast<T>(std::move(other.coefs[i]));
        }
        return *this;
    }

    // operator+= and operator+ in progress
};

// Deduction guide for deducing T and N
// perfect forwarding of arguments
template <typename... Args>
poly(Args&&...) -> poly<std::common_type_t<Args...>, sizeof...(Args)>;
