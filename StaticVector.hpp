#pragma once

#include <cstddef>
#include <stdexcept>

/*
StaticVector.hpp 实现了一个固定容量的 vector-like 容器。T 表示元素类型，N 表示容量，并且 N 在编译期确定。它不像 std::vector 那样动态扩容，
所以更适合 low-latency 场景。它支持 push_back、operator[]、size、capacity、begin 和 end，因此可以配合泛型算法和 range-based for 使用。
*/



template<typename T, std::size_t N>
class StaticVector {
private:
    T data_[N];
    std::size_t size_;

public:
    StaticVector() : size_(0) {}

    void push_back(const T& value) {
        if (size_ >= N) {
            throw std::out_of_range("StaticVector capacity exceeded");
        }
        data_[size_++] = value;
    }

    T& operator[](std::size_t index) {
        return data_[index];
    }

    const T& operator[](std::size_t index) const {
        return data_[index];
    }

    std::size_t size() const {
        return size_;
    }

    constexpr std::size_t capacity() const {
        return N;
    }

    T* begin() {
        return data_;
    }

    T* end() {
        return data_ + size_;
    }

    const T* begin() const {
        return data_;
    }

    const T* end() const {
        return data_ + size_;
    }
};