#ifndef POLY_H
#define POLY_H
#endif

#include <array>
#include <iostream>
#include <utility>

#define DEBUG 1

#if DEBUG
#define DBG_PRINT(x)                                                           \
    if (!std::is_constant_evaluated()) {                                       \
        std::cout << x << std::endl;                                           \
    }
#else
#define DBG_PRINT(x)
#endif

// TODO when N or M is 0 operators should return poly<T, 0>
// TODO maybe hide some things in the details namespace
// TODO probably some of the static_cast and convertible_to are wrong

template <typename U, typename T, std::size_t M, std::size_t N>
concept PolyConvertible = (M <= N) && std::is_convertible_v<U, T>;

// To be able to use the "poly" type name before its definition
template <typename T, std::size_t N> class poly;

// For recursive deduction of the base type of poly<...>
template <typename T>
struct BaseType {
    using type = T;
};
template <typename T, std::size_t N>
struct BaseType<poly<T, N>> {
    using type = typename BaseType<T>::type;
};
template <typename T>
using base_type = typename BaseType<T>::type;

template <typename T, std::size_t N = 0> class poly {

  private:
    std::array<T, N> coefs{};

  public:
    constexpr poly() { DBG_PRINT("empty constructor called"); }

    template <typename U, std::size_t M>
        requires PolyConvertible<U, T, M, N>
    constexpr poly(const poly<U, M>& other) {
        DBG_PRINT("copy constructor called");
        for (std::size_t i = 0; i < M; i++) {
            this->coefs[i] = static_cast<T>(other[i]);
        }
    }

    template <typename U, std::size_t M>
        requires PolyConvertible<U, T, M, N>
    constexpr poly(poly<U, M>&& other) {
        DBG_PRINT("move constructor called");
        for (std::size_t i = 0; i < M; i++) {
            this->coefs[i] = static_cast<T>(std::move(other[i]));
        }
    }

    template <typename U>
        requires std::convertible_to<U, T>
    constexpr poly(U&& arg) {
        DBG_PRINT("single constructor called");
        static_assert(N >= 1, "N cannot be smaller than 1");
        this->coefs[0] = static_cast<T>(std::move(arg));
    }

    template <typename... Args>
        requires(sizeof...(Args) <= N && (std::convertible_to<Args, T> && ...))
    constexpr poly(Args&&... args) : coefs{std::forward<Args>(args)...} {
        DBG_PRINT("variadic constructor called");
    }

    constexpr std::size_t size() const { return N; }

    constexpr T& operator[](std::size_t i) { return coefs[i]; }

    constexpr const T& operator[](std::size_t i) const { return coefs[i]; }

    // Copy assignment operator
    template <typename U, std::size_t M>
        requires PolyConvertible<U, T, M, N>
    constexpr poly& operator=(const poly<U, M>& other) {
        DBG_PRINT("copy assignment operator called");
        for (std::size_t i = 0; i < M; i++) {
            coefs[i] = static_cast<T>(other[i]);
        }
        return *this;
    }

    // Move assignment operator
    template <typename U, std::size_t M>
        requires PolyConvertible<U, T, M, N>
    constexpr poly& operator=(poly<U, M>&& other) {
        DBG_PRINT("move assignment operator called");
        for (std::size_t i = 0; i < M; i++) {
            coefs[i] = static_cast<T>(std::move(other[i]));
        }
        return *this;
    }

    template <typename U, std::size_t M>
        requires PolyConvertible<U, T, M, N>
    constexpr poly& operator+=(const poly<U, M>& other) {
        DBG_PRINT("operator+= for poly called");
        for (std::size_t i = 0; i < M; i++) {
            coefs[i] += static_cast<T>(other[i]);
        }
        return *this;
    }

    template <typename U>
        requires std::convertible_to<U, T>
    constexpr poly& operator+=(const U& other) {
        DBG_PRINT("operator+= for U called");
        coefs[0] += static_cast<T>(other);
        return *this;
    }

    template <typename U, std::size_t M>
        requires std::is_convertible_v<T, U> || std::is_convertible_v<U, T>
    constexpr auto operator+(const poly<U, M>& other) const {
        constexpr std::size_t res_size = std::max(N, M);
        poly<std::common_type_t<T, U>, res_size> result{*this};
        result += other;
        return result;
    }

    template <typename U, std::size_t M>
        requires PolyConvertible<U, T, M, N>
    constexpr poly& operator-=(const poly<U, M>& other) {
        DBG_PRINT("operator+= for poly called");
        for (std::size_t i = 0; i < M; i++) {
            coefs[i] -= static_cast<T>(other[i]);
        }
        return *this;
    }

    template <typename U>
        requires std::convertible_to<U, T>
    constexpr poly& operator-=(const U& other) {
        DBG_PRINT("operator-= for U called");
        coefs[0] -= static_cast<T>(other);
        return *this;
    }

    template <typename U, std::size_t M>
        requires std::is_convertible_v<T, U> || std::is_convertible_v<U, T>
    constexpr auto operator-(const poly<U, M>& other) const {
        constexpr std::size_t res_size = std::max(N, M);
        poly<std::common_type_t<T, U>, res_size> result{*this};
        result -= other;
        return result;
    }

    // Unary minus operator
    constexpr auto operator-() const {
        poly<T, N> result{};
        for (std::size_t i = 0; i < N; i++) {
            result[i] = -coefs[i];
        }
        return result;
    }

    template <typename U>
        requires std::convertible_to<U, T>
    constexpr poly& operator*=(const U& other) {
        DBG_PRINT("operator*= for U called");
        for (std::size_t i = 0; i < N; i++) {
            coefs[i] *= other;
        }
        return *this;
    }

    template <typename U, std::size_t M>
        requires std::is_convertible_v<T, U> || std::is_convertible_v<U, T>
    constexpr auto operator*(const poly<U, M>& other) const {
        constexpr std::size_t res_size = N + M - 1;
        poly<std::common_type_t<T, U>, res_size> result{};
        for (std::size_t i = 0; i < N; i++) {
            for (std::size_t j = 0; j < M; j++) {
                result[i + j] += coefs[i] * other[j];
            }
        }
        return result;
    }

    // Multiplication of a poly object with a scalar
    template <typename U>
        requires std::convertible_to<U, T>
    constexpr auto operator*(const U& value) const {
        poly result = *this;
        for (std::size_t i = 0; i < N; i++) {
            // result[i] = result[i] * value;
            result[i] *= value;
        }
        return result;
    }

    // Addition of a poly object with a scalar
    template <typename U>
        requires std::convertible_to<U, T>
    constexpr auto operator+(const U& value) const {
        poly result = *this;
        result[0] += static_cast<T>(value);
        return result;
    }

    // Subtraction of a poly object with a scalar
    template <typename U>
        requires std::convertible_to<U, T>
    constexpr auto operator-(const U& value) const {
        poly result = *this;
        result[0] -= static_cast<T>(value);
        return result;
    }

        // Funkcja pomocnicza dla at(...)
    template <typename U>
    constexpr U pow(const T& init, const U& base, std::size_t exp) const 
    requires std::convertible_to<base_type<U>, T> {
   		U res = init;
   		for (std::size_t i = 0; i < exp; ++i) {
   			res *= base;
   		}
   		return res;
   	}
   	
   	// k == 0
   	constexpr const poly<T>& at() const {
    	return *this;
    }
    
    // k == 1
    template <typename U>
    constexpr U at(const U& arg) const
    requires std::convertible_to<base_type<U>, T> {
    	if (N == 0) {
    		return T{};
    	}
    	
    	U res = this->coefs[0];
		for (std::size_t i = 1; i < N; ++i) {
			res += pow(this->coefs[i], arg, i);
    	}
    	return res;
    }
   	
   	// k > 1
    template <typename U, typename... Args>
    constexpr U at(const U& first, Args&&... args) const
    requires (std::convertible_to<base_type<U>, T> && (std::convertible_to<base_type<Args>, T> && ...)) {
        if (N == 0) {
        	return T{};
        }
        
        if (N == 1) {
       		// "nadmiarowe argumenty są ignorowane, gdyż zmienne 
       		// xi dla i>n po prostu nie występują w wielomianie."
        	return this->at(first);
        }
        
        U result = this->coefs[0];
		for (std::size_t i = 1; i < N; ++i) {
    		result += this->coefs[i].at(args...) * pow(first, i);
		}
		return result;
    }
};

// Non-member operator* for scalar multiplication (commutative property)
template <typename T, std::size_t N, typename U>
    requires std::convertible_to<U, T>
constexpr auto operator*(const U& value, const poly<T, N>& poly_obj) {
    return poly_obj * value;
}

// Non-member operator+ for scalar addition (commutative property)
template <typename T, std::size_t N, typename U>
    requires std::convertible_to<U, T>
constexpr auto operator+(const U& value, const poly<T, N>& poly_obj) {
    return poly_obj + value;
}

// Non-member operator- for scalar subtraction (commutative property)
template <typename T, std::size_t N, typename U>
    requires std::convertible_to<U, T>
constexpr auto operator-(const U& value, const poly<T, N>& poly_obj) {
    poly<T, N> result = poly_obj;
    result[0] = static_cast<T>(value) - result[0];
    return result;
}

// Deduction guide for deducing T and N
// perfect forwarding of arguments
template <typename... Args>
poly(Args&&...) -> poly<std::common_type_t<Args...>, sizeof...(Args)>;

template <typename U, std::size_t M>
constexpr auto const_poly(const poly<U, M>& p) {
    DBG_PRINT("const_poly constructor called");
    return poly<poly<U, M>, 1>{p};
}

// std::common_type specialization
namespace std {
template <typename T, std::size_t N, typename U, std::size_t M>
struct common_type<poly<T, N>, poly<U, M>> {
    using type = poly<std::common_type_t<T, U>, std::max(N, M)>;
};

template <typename T, std::size_t N, typename U>
struct common_type<poly<T, N>, U> {
    using type = poly<std::common_type_t<T, U>, N>;
};

template <typename T, typename U, std::size_t M>
struct common_type<T, poly<U, M>> {
    using type = poly<std::common_type_t<T, U>, M>;
};
} // namespace std
