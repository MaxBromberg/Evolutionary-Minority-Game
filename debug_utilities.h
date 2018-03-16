#pragma once

#include <iostream>
#include <type_traits>
#include <fstream>

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

static inline void debug_console_message(){ std::cout << "Line Accessed" << std::endl; }

//template <typename T>
//void debug_write (const T& v, typename std::enable_if<std::is_arithmetic<T>::value>::type* = 0 ) {
//    ofstream debug_file("Debug File");
//    debug_file << v <<endl;
//}
//
//static inline void debug_write (bool v) {
//    ofstream debug_file("Debug File");
//    debug_file << (v ? "true" : "false") << endl;
//}
//
//template <typename T>
//void debug_write (const T& v, typename std::enable_if<std::is_arithmetic<T>::value == false>::type* = 0 ) {
//    ofstream debug_file("Debug File");
//    debug_file << "[ ";
//
//    for (auto iter = v.cbegin(); iter != v.cend();) {
//        debug_print (*iter);
//        if (++iter != v.cend()) {
//            debug_file << ", ";
//        }
//    }
//    debug_file << "]" << endl;
//}
