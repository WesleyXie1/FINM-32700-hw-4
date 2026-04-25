#pragma once

#include <iostream>
#include <type_traits>
#include <utility>

/*
这个文件展示了 C++ 模板元编程。Factorial 和 Fibonacci 不是普通函数，而是在编译期通过模板递归计算。
IsOdd 用 SFINAE 判断一个 compile-time constant 是否是奇数，并且只在奇数时启用 print_if_odd。print_all 是 variadic template，
可以打印任意数量、任意类型的参数。
*/

// Factorial
template<int N>
struct Factorial {
    static_assert(N >= 0, "Factorial is only defined for non-negative integers");
    static constexpr int value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

// Fibonacci
template<int N>
struct Fibonacci {
    static_assert(N >= 0, "Fibonacci is only defined for non-negative integers");
    static constexpr int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

template<>
struct Fibonacci<0> {
    static constexpr int value = 0;
};

template<>
struct Fibonacci<1> {
    static constexpr int value = 1;
};

// IsOdd trait for compile-time integral constants
template<typename T, typename = void>
struct IsOdd : std::false_type {};

template<typename T>
struct IsOdd<T, std::void_t<decltype(T::value)>>
    : std::integral_constant<bool, (T::value % 2 != 0)> {};

// void_t example detect price member
template<typename, typename = void>
struct HasPriceMember : std::false_type {};

template<typename T>
struct HasPriceMember<T, std::void_t<decltype(std::declval<T>().price)>>
    : std::true_type {};

// print_if_odd only enabled if T::value is odd
template<typename T>
std::enable_if_t<IsOdd<T>::value, void>
print_if_odd(T) {
    std::cout << T::value << " is odd\n";
}

// Variadic template print_all
inline void print_all() {
    std::cout << '\n';
}

template<typename First, typename... Rest>
void print_all(const First& first, const Rest&... rest) {
    std::cout << first;

    if constexpr (sizeof...(rest) > 0) {
        std::cout << " ";
        print_all(rest...);
    } else {
        std::cout << '\n';
    }
}

// Compile time validation
static_assert(Factorial<5>::value == 120, "Factorial<5> should be 120");
static_assert(Fibonacci<7>::value == 13, "Fibonacci<7> should be 13");