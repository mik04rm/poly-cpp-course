#ifndef POLY_H
#define POLY_H
#endif

#include <array>
#include <iostream>
#include <utility>
#include <sstream>

#define DEBUG 1

#if DEBUG
#define DBG_PRINT(x)                                                           \
    if (!std::is_constant_evaluated()) {                                       \
        std::cout << x << std::endl;                                           \
    }
#else
#define DBG_PRINT(x)
#endif

template <typename U, typename T, std::size_t M, std::size_t N>
concept PolyConvertible = (M <= N) && std::is_convertible_v<U, T>;

// To be able to use the "poly" type name before its definition
template <typename T, std::size_t N> class poly;

// For recursive deduction of the base type of poly<...>.
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

    std::ostringstream oss(int var) const {
	std::ostringstream res;
	for (std::size_t i = 0; i < N; ++i) {
		const T& a = this->coefs[i];
		if constexpr (requires { typename T::value_type; }) {
			res << "(" << a.oss(var+1).str() << ")";
		}
		else {
			res << a;
		}
		if (i > 0) {
			res << "*x" << var;
		}
		if (i > 1) {
			res << "^" << i;
		}
		if (i < N-1) {
			res << " + ";
		}
	}
	return res;
    }

  public:
    std::string to_string() const {
	return this->oss(1).str();
    }

    friend std::ostream& operator<<(std::ostream& os, const poly& p) {
      	return os << "poly<" << typeid(T).name() << ", " << N << ">";
    }

    constexpr poly() = default;

    template <typename U, std::size_t M>
        requires PolyConvertible<U, T, M, N>
    constexpr poly(const poly<U, M>& other) {
        for (std::size_t i = 0; i < M; i++) {
            this->coefs[i] = other[i];
        }
    }

    template <typename U, std::size_t M>
        requires PolyConvertible<U, T, M, N>
    constexpr poly(poly<U, M>&& other) {
        for (std::size_t i = 0; i < M; i++) {
            this->coefs[i] = std::move(other[i]);
        }
    }

    template <typename U>
        requires std::convertible_to<U, T>
    constexpr poly(U&& arg) {
        static_assert(N >= 1, "N cannot be smaller than 1");
        this->coefs[0] = std::forward<U>(arg);
    }

    template <typename... Args>
        requires(sizeof...(Args) >= 2 && sizeof...(Args) <= N && (std::convertible_to<Args, T> && ...))
    constexpr poly(Args&&... args) : coefs{static_cast<T>(std::forward<Args>(args))...} {
    }

    constexpr std::size_t size() const { return N; }

    constexpr T& operator[](std::size_t i) { return coefs[i]; }

    constexpr const T& operator[](std::size_t i) const { return coefs[i]; }

    // Copy assignment operator.
    template <typename U, std::size_t M>
        requires PolyConvertible<U, T, M, N>
    constexpr poly& operator=(const poly<U, M>& other) {
        for (std::size_t i = 0; i < M; i++) {
            coefs[i] = other[i];
        }
        return *this;
    }

    // Move assignment operator.
    template <typename U, std::size_t M>
        requires PolyConvertible<U, T, M, N>
    constexpr poly& operator=(poly<U, M>&& other) {
        for (std::size_t i = 0; i < M; i++) {
            coefs[i] = std::move(other[i]);
        }
        return *this;
    }

    template <typename U, std::size_t M>
        requires PolyConvertible<U, T, M, N>
    constexpr poly& operator+=(const poly<U, M>& other) {
        for (std::size_t i = 0; i < M; i++) {
            coefs[i] += other[i];
        }
        return *this;
    }

    template <typename U>
        requires std::convertible_to<U, T>
    constexpr poly& operator+=(const U& other) {
        coefs[0] += other;
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
        for (std::size_t i = 0; i < M; i++) {
            coefs[i] -= other[i];
        }
        return *this;
    }

    template <typename U>
        requires std::convertible_to<U, T>
    constexpr poly& operator-=(const U& other) {
        coefs[0] -= other;
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
        for (std::size_t i = 0; i < N; i++) {
            coefs[i] *= other;
        }
        return *this;
    }

    template <typename U, std::size_t M>
        requires std::is_convertible_v<T, U> || std::is_convertible_v<U, T>
    constexpr auto operator*(const poly<U, M>& other) const {
        if constexpr (N == 0 || M == 0) {
            return poly<std::common_type_t<T, U>, 0>{};
        }
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
        poly<std::common_type_t<U, T>, N> result = *this;
        for (std::size_t i = 0; i < N; i++) {
            result[i] *= value;
        }
        return result;
    }

    // Addition of a poly object with a scalar
    template <typename U>
        requires std::convertible_to<U, T>
    constexpr auto operator+(const U& value) const {
        poly<std::common_type_t<U, T>, N> result = *this;
        result[0] += value;
        return result;
    }

    // Subtraction of a poly object with a scalar
    template <typename U>
        requires std::convertible_to<U, T>
    constexpr auto operator-(const U& value) const {
        poly<std::common_type_t<U, T>, N> result = *this;
        result[0] -= value;
        return result;
    }

    template <typename U>
    requires std::convertible_to<U, T>
    static constexpr U eval_term(const T& coef, const U& pow_base, std::size_t pow_exp) {
   		U res = coef;
		for (std::size_t i = 1; i <= pow_exp; ++i) {
   			res *= pow_base;
   		}
   		return res;
   	}
   	
   	template <typename U, std::size_t M>
   	requires std::convertible_to<base_type<U>, T>
   	static constexpr auto eval_term(const T& coef, const poly<U, M>& pow_base, std::size_t pow_exp) {
   		constexpr std::size_t res_size = (N-1)*(M-1) + 1;
        using res_type = poly<std::common_type_t<T, U>, res_size>;
        res_type res(coef);
    	
    	for (std::size_t i = 1; i <= pow_exp; ++i) {
    		res *= pow_base;
    	}
    	
    	return res;
   	}
   	
   	// k == 0
   	constexpr const poly<T, N>& at() const {
    	return *this;
    }
    
    // k == 1
    template <typename U>
    requires std::convertible_to<U, T>
    constexpr U at(const U& arg) const {
    	if (N == 0) {
    		return T{};
    	}
    	
    	U res = this->coefs[0]; 
    	
		for (std::size_t i = 1; i < N; ++i) {
			U term = eval_term(this->coefs[i], arg, i);
			res += term;
    	}
    	
    	return res;
    }
    
    template <typename U, std::size_t M>
    requires std::convertible_to<base_type<U>, T>
    constexpr auto at(const poly<U, M>& arg) const {
       	constexpr std::size_t res_size = (N-1)*(M-1) + 1;
        using res_type = poly<std::common_type_t<T, U>, res_size>;
        
        if (N == 0) {
    		return res_type();
    	}
        res_type res(this->coefs[0]);
    	
    	for (std::size_t i = 1; i < N; ++i) {
			auto term = eval_term(this->coefs[i], arg, i);
			res += term;
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

// Non-member operator* for scalar multiplication. 
template <typename T, std::size_t N, typename U>
    requires std::convertible_to<U, T>
constexpr auto operator*(const U& value, const poly<T, N>& p) {
    return p * value;
}

// Non-member operator+ for scalar addition. 
template <typename T, std::size_t N, typename U>
    requires std::convertible_to<U, T>
constexpr auto operator+(const U& value, const poly<T, N>& p) {
    return p + value;
}

// Non-member operator- for scalar subtraction. 
template <typename T, std::size_t N, typename U>
    requires std::convertible_to<U, T>
constexpr auto operator-(const U& value, const poly<T, N>& p) {
    return -(p - value);
}

template <typename U, std::size_t M>
constexpr auto const_poly(const poly<U, M>& p) {
    DBG_PRINT("const_poly constructor called");
    return poly<poly<U, M>, 1>{p};
}
namespace detail {

template <typename T> struct is_poly_impl : std::false_type {};

template <typename T, std::size_t N>
struct is_poly_impl<poly<T, N>> : std::true_type {};

template <typename T>
struct is_poly : is_poly_impl<std::remove_cvref_t<T>> {};

template <typename T>
inline constexpr bool is_poly_v = is_poly<T>::value;
} // namespace detail

// std::common_type specialization for poly.
namespace std {

template <typename T, std::size_t N, typename U, std::size_t M>
struct common_type<poly<T, N>, poly<U, M>> {
    using type = poly<std::common_type_t<T, U>, std::max(N, M)>;
};

template <typename T, std::size_t N, typename U>
    requires (!detail::is_poly_v<U>)
struct common_type<poly<T, N>, U> {
    using type = poly<std::common_type_t<T, U>, N>;
};

template <typename T, typename U, std::size_t M>
    requires (!detail::is_poly_v<T>)
struct common_type<T, poly<U, M>> {
    using type = poly<std::common_type_t<T, U>, M>;
};

} // namespace std

template <typename T, typename... Args>
poly(T&&,
     Args&&...) -> poly<std::common_type_t<T, Args...>, 1 + sizeof...(Args)>;

template <typename T1, std::size_t N1, typename T2, std::size_t N2>
constexpr auto cross(const poly<T1, N1>& p, const poly<T2, N2>& q) {
    constexpr std::size_t N = N1;
    poly<decltype(cross(p[0], q)), N> result{};
    for (std::size_t i = 0; i < N1; ++i) {
        result[i] = cross(p[i], q);
    }
    return result;
}

template <typename T1, std::size_t N1, typename T2>
constexpr auto cross(const poly<T1, N1>& p, const T2& q) {
    poly<decltype(p[0] * q), N1> result{};
    for (std::size_t i = 0; i < N1; ++i) {
        result[i] = p[i] * q;
    }
    return result;
}

template <typename T1, typename T2, std::size_t N2>
constexpr auto cross(const T1& p, const poly<T2, N2>& q) {
    poly<decltype(p * q[0]), N2> result{};
    for (std::size_t i = 0; i < N2; ++i) {
        result[i] = p * q[i];
    }
    return result;
}

template <typename T1, typename T2>
constexpr auto cross(const T1& a, const T2& b) {
    return a * b;
}
