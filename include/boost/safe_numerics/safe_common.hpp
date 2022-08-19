#ifndef BOOST_NUMERIC_SAFE_COMMON_HPP
#define BOOST_NUMERIC_SAFE_COMMON_HPP

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>

namespace boost {
namespace safe_numerics {

// default implementations for required meta-functions
// 元函数用于判断是否是安全数值类型(即safe_base)
template<typename T>
struct is_safe : public std::false_type
{};

template<typename T>
struct base_type {
    using type = T;
};

template<class T>
constexpr const typename base_type<T>::type & base_value(const T & t) {
    return static_cast<const typename base_type<T>::type & >(t);
}

// 获取类型T的类型提升策略
// 默认的类型提升策略类型为void
template<typename T>
struct get_promotion_policy {
    using type = void;
};

// 获取类型T的异常处理策略
// 默认的异常处理策略类型为void
template<typename T>
struct get_exception_policy {
    using type = void;
};


} // safe_numerics
} // boost

#endif // BOOST_NUMERIC_SAFE_COMMON_HPP
