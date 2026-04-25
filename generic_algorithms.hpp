#pragma once

template<typename InputIterator, typename Predicate>
InputIterator find_if_custom(InputIterator first, InputIterator last, Predicate pred) {
    while (first != last) {
        if (pred(*first)) {
            return first;
        }
        ++first;
    }
    return last;
}