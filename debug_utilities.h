#pragma once

#include <iostream>
#include <type_traits>

template <typename T>
void debug_print (const T& v, typename std::enable_if<std::is_arithmetic<T>::value>::type* = 0 ) {
    std::cout << v;
}

static inline void debug_print (bool v) {
    std::cout << (v ? "true" : "false");
}

template <typename T>
void debug_print (const T& v, typename std::enable_if<std::is_arithmetic<T>::value == false>::type* = 0 ) {
    std::cout << "[ ";

    for (auto iter = v.cbegin(); iter != v.cend();) {
        debug_print (*iter);
        if (++iter != v.cend()) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}
