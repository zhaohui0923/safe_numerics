#ifndef BOOST_SAFE_NUMERICS_NATIVE_HPP
#define BOOST_SAFE_NUMERICS_NATIVE_HPP

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>
#include <limits>

// policy which creates results types and values equal to that of C++ promotions.
// When used in conjunction with a desired exception policy, traps errors but
// does not otherwise alter the results produced by the program using it.
namespace boost {
namespace safe_numerics {

// 与C++原生兼容的类型提升策略
// 仅负责二元运算的结果类型推导
struct native {
public:
    // arithmetic operators

    // 与C++原生兼容的加法类型提升策略
    // T: 加法左操作数的类型
    // U: 加法右操作数的类型
    // 这是一个模版元函数，返回值为类型
    template<typename T, typename U>
    struct addition_result {
        using type = decltype(
            typename base_type<T>::type()
            + typename base_type<U>::type()
        );
    };

    // 与C++原生兼容的减法类型提升策略
    // T: 减法左操作数的类型
    // U: 减法右操作数的类型
    // 这是一个模版元函数，返回值为类型
    template<typename T, typename U>
    struct subtraction_result {
        using type = decltype(
            typename base_type<T>::type()
            - typename base_type<U>::type()
        );
    };

    // 与C++原生兼容的乘法类型提升策略
    // T: 乘法左操作数的类型
    // U: 乘法右操作数的类型
    // 这是一个模版元函数，返回值为类型
    template<typename T, typename U>
    struct multiplication_result {
        using type = decltype(
            typename base_type<T>::type()
            * typename base_type<U>::type()
        );
    };

    // 与C++原生兼容的除法类型提升策略
    // T: 除法左操作数的类型
    // U: 除法右操作数的类型
    // 这是一个模版元函数，返回值为类型
    template<typename T, typename U>
    struct division_result {
        using type = decltype(
            typename base_type<T>::type()
            / typename base_type<U>::type()
        );
    };

    // 与C++原生兼容的取模类型提升策略
    // T: 取模左操作数的类型
    // U: 取模右操作数的类型
    // 这是一个模版元函数，返回值为类型
    template<typename T, typename U>
    struct modulus_result {
        using type = decltype(
            typename base_type<T>::type()
            % typename base_type<U>::type()
        );
    };

    // 与C++原生兼容的比较类型提升策略
    // T: 比较左操作数的类型
    // U: 比较右操作数的类型
    // 这是一个模版元函数，返回值为类型
    // note: comparison_result (<, >, ...) is special.
    // The return value is always a bool.  The type returned here is
    // the intermediate type applied to make the values comparable.
    template<typename T, typename U>
    struct comparison_result {
        using type = decltype(
            typename base_type<T>::type()
            + typename base_type<U>::type()
        );
    };

    // 与C++原生兼容的左移运算类型提升策略
    // T: 左移左操作数的类型
    // U: 左移右操作数的类型
    // 这是一个模版元函数，返回值为类型
    // shift operators
    template<typename T, typename U>
    struct left_shift_result {
        using type = decltype(
            typename base_type<T>::type()
            << typename base_type<U>::type()
        );
    };

    // 与C++原生兼容的右移运算类型提升策略
    // T: 右移左操作数的类型
    // U: 右移右操作数的类型
    // 这是一个模版元函数，返回值为类型
    template<typename T, typename U>
    struct right_shift_result {
        using type = decltype(
            typename base_type<T>::type()
            >> typename base_type<U>::type()
        );
    };

    // bitwise operators

    // 与C++原生兼容的按位或运算类型提升策略
    // T: 按位或左操作数的类型
    // U: 按位或右操作数的类型
    // 这是一个模版元函数，返回值为类型
    template<typename T, typename U>
    struct bitwise_or_result {
        using type = decltype(
            typename base_type<T>::type()
            | typename base_type<U>::type()
        );
    };

    // 与C++原生兼容的按位与运算类型提升策略
    // T: 按位与左操作数的类型
    // U: 按位与右操作数的类型
    // 这是一个模版元函数，返回值为类型
    template<typename T, typename U>
    struct bitwise_and_result {
        using type = decltype(
            typename base_type<T>::type()
            & typename base_type<U>::type()
        );
    };

    // 与C++原生兼容的按位异或运算类型提升策略
    // T: 按位异或左操作数的类型
    // U: 按位异或右操作数的类型
    // 这是一个模版元函数，返回值为类型
    template<typename T, typename U>
    struct bitwise_xor_result {
        using type = decltype(
            typename base_type<T>::type()
            ^ typename base_type<U>::type()
        );
    };
};

} // safe_numerics
} // boost

#endif // BOOST_SAFE_NUMERICS_NATIVE_HPP
