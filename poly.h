#ifndef POLY_H
#define POLY_H
#endif

#include <array>
#include <utility>
#include <iostream>

#define DEBUG 1

template <typename T, std::size_t N = 0> 
class poly  {

private:
    std::array<T, N> coefs{};

public:
    
    constexpr poly() {
        if (DEBUG)
            std::cout << "empty constructor called" << std::endl;
    }

    template <typename U, std::size_t M>
    constexpr poly(const poly<U, M>& other) 
    requires ((M <= N) && std::is_convertible_v<U, T>) {
        if (DEBUG) 
            std::cout << "copy constructor called" << std::endl;
        for (std::size_t i = 0; i < M; ++i) {
            this->coefs[i] = static_cast<T>(other.coefs[i]);
        }
    }

    template <typename U, std::size_t M>
    constexpr poly(poly<U, M>&& other) 
    requires ((M <= N) && std::is_convertible_v<U, T>) {
        if (DEBUG) 
            std::cout << "move constructor called" << std::endl;
        for (std::size_t i = 0; i < M; ++i) {
            this->coefs[i] = static_cast<T>(std::move(other.coefs[i]));
        }
    }

    template <typename U>
    constexpr poly(U&& arg) 
    requires std::convertible_to<U,T> {
        if (DEBUG) 
            std::cout << "single constructor called" << std::endl;
        static_assert(N >= 1, "N cannot be smaller than 1");
        this->coefs[0] = static_cast<T>(std::move(arg));
    }

    template <typename... Args>
    constexpr poly(Args&&... args)
    requires (sizeof...(Args) <= N && (std::convertible_to<Args, T> && ...))
    : coefs{std::forward<Args>(args)...} {
        if (DEBUG) 
            std::cout << "variadic constructor called" << std::endl;
    }

    template <typename U, std::size_t M>
    constexpr static auto const_poly(const poly<U,M>& p) {
        if (DEBUG) 
            std::cout << "const_poly constructor called" << std::endl;
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
};
