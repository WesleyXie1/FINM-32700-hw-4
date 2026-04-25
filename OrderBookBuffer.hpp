#pragma once

#include <cstddef>
#include <cstring>
#include <iostream>
#include <mutex>
#include <new>
#include <type_traits>
#include <utility>

/*
OrderBookBuffer 用模板参数把 memory policy 和 threading policy 抽出来，使同一个容器可以在不同低延迟场景下复用。
*/



// Heap allocator policy
struct HeapAllocator {
    template<typename T>
    T* allocate(std::size_t n) {
        return static_cast<T*>(::operator new(sizeof(T) * n));
    }

    template<typename T>
    void deallocate(T* ptr) {
        ::operator delete(ptr);
    }
};

// Stack allocator policy
// Uses fixed internal storage. Capacity must not exceed MaxSize.
template<std::size_t MaxSize = 1024>
struct StackAllocator {
    alignas(std::max_align_t) unsigned char storage[MaxSize * 128];

    template<typename T>
    T* allocate(std::size_t n) {
        if (sizeof(T) * n > sizeof(storage)) {
            throw std::bad_alloc();
        }
        return reinterpret_cast<T*>(storage);
    }

    template<typename T>
    void deallocate(T*) {
    }
};

// zero init allocator
struct ZeroInitAllocator {
    template<typename T>
    T* allocate(std::size_t n) {
        T* ptr = static_cast<T*>(::operator new(sizeof(T) * n));
        std::memset(ptr, 0, sizeof(T) * n);
        return ptr;
    }

    template<typename T>
    void deallocate(T* ptr) {
        ::operator delete(ptr);
    }
};

// No locking policy
struct NoLock {
    void lock() {}
    void unlock() {}
};

// Mutex locking policy
struct MutexLock {
private:
    std::mutex mutex_;

public:
    void lock() {
        mutex_.lock();
    }

    void unlock() {
        mutex_.unlock();
    }
};

// RAII lock guard for any lock policy
template<typename LockPolicy>
class PolicyGuard {
private:
    LockPolicy& lock_;

public:
    explicit PolicyGuard(LockPolicy& lock) : lock_(lock) {
        lock_.lock();
    }

    ~PolicyGuard() {
        lock_.unlock();
    }
};

// Detect price member
template<typename, typename = std::void_t<>>
struct HasPrice : std::false_type {};

template<typename T>
struct HasPrice<T, std::void_t<decltype(std::declval<T>().price)>>
    : std::true_type {};

// Policy-based OrderBookBuffer
template<typename T, typename AllocatorPolicy, typename ThreadingPolicy>
class OrderBookBuffer {
private:
    AllocatorPolicy allocator_;
    ThreadingPolicy lock_;
    T* data_;
    std::size_t capacity_;
    std::size_t size_;

public:
    explicit OrderBookBuffer(std::size_t capacity)
        : data_(nullptr), capacity_(capacity), size_(0) {
        data_ = allocator_.template allocate<T>(capacity_);
    }

    ~OrderBookBuffer() {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        allocator_.template deallocate<T>(data_);
    }

    OrderBookBuffer(const OrderBookBuffer&) = delete;
    OrderBookBuffer& operator=(const OrderBookBuffer&) = delete;

    template<typename U = T>
    std::enable_if_t<HasPrice<U>::value, void>
    add_order(const T& order) {
        PolicyGuard<ThreadingPolicy> guard(lock_);

        if (size_ >= capacity_) {
            throw std::out_of_range("OrderBookBuffer is full");
        }

        new (&data_[size_]) T(order);
        ++size_;
    }

    void print_orders() const {
        for (std::size_t i = 0; i < size_; ++i) {
            std::cout << "Order id=" << data_[i].id
                      << ", price=" << data_[i].price
                      << ", qty=" << data_[i].qty << '\n';
        }
    }

    std::size_t size() const {
        return size_;
    }

    std::size_t capacity() const {
        return capacity_;
    }
};

// Bonus RingBuffer with no dynamic allocation
template<typename T, std::size_t Size>
class RingBuffer {
private:
    static_assert(std::is_trivially_copyable<T>::value,
                  "T must be trivially copyable");

    T data_[Size];
    std::size_t head_;
    std::size_t tail_;
    std::size_t count_;

public:
    RingBuffer() : head_(0), tail_(0), count_(0) {}

    bool push(const T& value) {
        if (count_ == Size) {
            return false;
        }

        data_[tail_] = value;
        tail_ = (tail_ + 1) % Size;
        ++count_;
        return true;
    }

    bool pop(T& out) {
        if (count_ == 0) {
            return false;
        }

        out = data_[head_];
        head_ = (head_ + 1) % Size;
        --count_;
        return true;
    }

    std::size_t size() const {
        return count_;
    }

    bool empty() const {
        return count_ == 0;
    }
};