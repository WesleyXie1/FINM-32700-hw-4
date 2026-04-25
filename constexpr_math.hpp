#pragma once

#include <cmath>

/*
这个文件用 constexpr 写了一些可以在编译期计算的函数，包括 square、factorial、Fibonacci 和 price_bucket。和模板元编程相比，constexpr 更像普通函数，
既可以编译期用，也可以运行期用。price_bucket 模拟交易里的 tick size，把价格向下归到最近的 0.05 档位。static_assert 用来证明这些计算在编译期就能通过检查。
*/

constexpr int square(int x) {
    return x * x;
}

constexpr int factorial_constexpr(int n) {
    return n <= 1 ? 1 : n * factorial_constexpr(n - 1);
}

constexpr int fibonacci_constexpr(int n) {
    return n <= 1 ? n : fibonacci_constexpr(n - 1) + fibonacci_constexpr(n - 2);
}

// Round price down to nearest 0.05
constexpr double price_bucket(double price) {
    return static_cast<int>(price / 0.05) * 0.05;
}

static_assert(square(5) == 25, "square(5) should be 25");
static_assert(factorial_constexpr(5) == 120, "factorial_constexpr(5) should be 120");
static_assert(fibonacci_constexpr(7) == 13, "fibonacci_constexpr(7) should be 13");
static_assert(price_bucket(101.73) > 101.699 && price_bucket(101.73) < 101.701,
              "price_bucket(101.73) should be close to 101.70");