#include <iostream>
#include <type_traits>

#include "metaprogramming.hpp"
#include "constexpr_math.hpp"
#include "StaticVector.hpp"
#include "generic_algorithms.hpp"
#include "OrderBookBuffer.hpp"

struct Order {
    int id;
    double price;
    int qty;
};

std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << "{id=" << order.id
       << ", price=" << order.price
       << ", qty=" << order.qty << "}";
    return os;
}

int main() {
    std::cout << "HFT Template Homework\n\n";

    // Part 1  Template metaprogramming
    std::cout << "Part 1: TMP\n";
    std::cout << "Factorial<5> = " << Factorial<5>::value << '\n';
    std::cout << "Fibonacci<7> = " << Fibonacci<7>::value << '\n';

    using Seven = std::integral_constant<int, 7>;
    print_if_odd(Seven{});

    print_all("Order", 101, 99.95, "AAPL");

    // Part 2  constexpr math
    std::cout << "\nPart 2: constexpr\n";
    constexpr int Size = square(5);
    int arr[Size] = {};

    std::cout << "square(5) = " << square(5) << '\n';
    std::cout << "factorial_constexpr(5) = " << factorial_constexpr(5) << '\n';
    std::cout << "fibonacci_constexpr(7) = " << fibonacci_constexpr(7) << '\n';
    std::cout << "price_bucket(101.73) = " << price_bucket(101.73) << '\n';
    std::cout << "Array size from constexpr = " << sizeof(arr) / sizeof(arr[0]) << '\n';

    // Part 3  StaticVector and generic algorithms
    std::cout << "\nPart 3: StaticVector + find_if\n";

    StaticVector<Order, 10> orders;
    orders.push_back({1, 99.95, 25});
    orders.push_back({2, 100.10, 30});
    orders.push_back({3, 101.25, 17});
    orders.push_back({4, 98.70, 40});
    orders.push_back({5, 102.00, 55});

    std::cout << "Orders with price > 100:\n";
    for (const auto& order : orders) {
        if (order.price > 100.0) {
            std::cout << order << '\n';
        }
    }

    auto first_divisible_by_10 = find_if_custom(
        orders.begin(),
        orders.end(),
        [](const Order& order) {
            return order.qty % 10 == 0;
        }
    );

    if (first_divisible_by_10 != orders.end()) {
        std::cout << "First order with quantity divisible by 10: "
                  << *first_divisible_by_10 << '\n';
    }

    auto first_price_above_100 = find_if_custom(
        orders.begin(),
        orders.end(),
        [](const Order& order) {
            return order.price > 100.0;
        }
    );

    if (first_price_above_100 != orders.end()) {
        std::cout << "First order with price > 100: "
                  << *first_price_above_100 << '\n';
    }

    // Part 4 Policy-based OrderBookBuffer
    std::cout << "\nPart 4: Policy-based OrderBookBuffer\n";

    OrderBookBuffer<Order, StackAllocator<>, NoLock> book1(10);
    book1.add_order({101, 100.25, 50});
    book1.add_order({102, 100.30, 75});

    std::cout << "book1: StackAllocator + NoLock\n";
    book1.print_orders();

    OrderBookBuffer<Order, HeapAllocator, MutexLock> book2(10);
    book2.add_order({201, 99.80, 100});
    book2.add_order({202, 100.05, 200});

    std::cout << "book2: HeapAllocator + MutexLock\n";
    book2.print_orders();

    OrderBookBuffer<Order, ZeroInitAllocator, NoLock> book3(5);
    book3.add_order({301, 101.50, 10});

    std::cout << "book3: ZeroInitAllocator + NoLock\n";
    book3.print_orders();

    // Bonus RingBuffer
    std::cout << "\nBonus: RingBuffer\n";
    RingBuffer<Order, 3> ring;
    ring.push({401, 99.10, 5});
    ring.push({402, 99.20, 15});

    Order popped{};
    if (ring.pop(popped)) {
        std::cout << "Popped from ring buffer: " << popped << '\n';
    }

    return 0;
}