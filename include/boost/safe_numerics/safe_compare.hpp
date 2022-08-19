#ifndef BOOST_NUMERIC_SAFE_COMPARE_HPP
#define BOOST_NUMERIC_SAFE_COMPARE_HPP

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>
#include <limits>

namespace boost {
namespace safe_numerics {
namespace safe_compare {

////////////////////////////////////////////////////
// safe comparison on primitive integral types
namespace safe_compare_detail {
    template<typename T>
    using make_unsigned = typename std::conditional<
        // 判断T是否为有符号类型
        std::is_signed<T>::value,  
        // 当T为有符号类型时，利用标准库的make_unsigned将T转换为无符号类型
        std::make_unsigned<T>,
        // 当T为无符号时，无需进行类型转换
        T
    >::type;

    // both arguments unsigned or signed
    template<bool TS, bool US>
    struct less_than {
        template<class T, class U>
        constexpr static bool invoke(const T & t, const U & u){
            // 两个操作数符号相同，直接比较
            return t < u;
        }
    };

    // T unsigned, U signed
    template<>
    struct less_than<false, true> {
        template<class T, class U>
        constexpr static bool invoke(const T & t, const U & u){
            return
                (u < 0) ?
                    false
                :
                    less_than<false, false>::invoke(
                        t,
                        static_cast<const typename make_unsigned<U>::type &>(u)
                    )
                ;
        }
    };
    // T signed, U unsigned
    template<>
    struct less_than<true, false> {
        template<class T, class U>
        constexpr static bool invoke(const T & t, const U & u){
            return
                (t < 0) ?
                    true
                :
                    less_than<false, false>::invoke(
                        static_cast<const typename make_unsigned<T>::type &>(t),
                        u
                    )
                ;
        }
    };
} // safe_compare_detail

// # Description
// Compare two primitive integers. These functions will return a correct result
// regardless of the type of the operands. Specifically it is guaranteed to
// return the correct arithmetic result when comparing signed and unsigned types
// of any size. It does not follow the standard C/C++ procedure of converting
// the operands to some common type then doing the compare. So it is not
// equivalent to the C/C++ binary operations <, >, >=, <=, ==, != and shouldn't
// be used by user programs which should be portable to standard C/C++ integer
// arithmetic. The functions are free functions defined inside the namespace
// boost::numeric::safe_compare.
//
// # Type requirements
// All template parameters of the functions must be C/C++ built-in integer
// types, char, int ....
//
// # Complexity
// Each function performs one and only one arithmetic operation.
// 
// lhs和rhs都是整型数的情况
template<class T, class U>
typename std::enable_if<
    std::is_integral<T>::value && std::is_integral<U>::value,
    bool
>::type
constexpr inline less_than(const T & lhs, const U & rhs) {
    return safe_compare_detail::less_than<
        std::is_signed<T>::value,
        std::is_signed<U>::value
    >:: invoke(lhs, rhs);
}

template<class T, class U>
typename std::enable_if<
    std::is_floating_point<T>::value && std::is_floating_point<U>::value,
    bool
>::type
constexpr inline less_than(const T & lhs, const U & rhs) {
    return lhs < rhs;
}

template<class T, class U>
constexpr inline bool greater_than(const T & lhs, const U & rhs) {
    return less_than(rhs, lhs);
}

template<class T, class U>
constexpr inline bool less_than_equal(const T & lhs, const U & rhs) {
    return ! greater_than(lhs, rhs);
}

template<class T, class U>
constexpr inline bool greater_than_equal(const T & lhs, const U & rhs) {
    return ! less_than(lhs, rhs);
}

namespace safe_compare_detail {
    // both arguments unsigned or signed
    template<bool TS, bool US>
    struct equal {
        template<class T, class U>
        constexpr static bool invoke(const T & t, const U & u){
            return t == u;
        }
    };

    // T unsigned, U signed
    template<>
    struct equal<false, true> {
        template<class T, class U>
        constexpr static bool invoke(const T & t, const U & u){
            return
                (u < 0) ?
                    false
                :
                    equal<false, false>::invoke(
                        t,
                        static_cast<const typename make_unsigned<U>::type &>(u)
                    )
                ;
        }
    };
    // T signed, U unsigned
    template<>
    struct equal<true, false> {
        template<class T, class U>
        constexpr static bool invoke(const T & t, const U & u){
            return
                (t < 0) ?
                    false
                :
                    equal<false, false>::invoke(
                        static_cast<const typename make_unsigned<T>::type &>(t),
                        u
                    )
                ;
        }
    };
} // safe_compare_detail

template<class T, class U>
typename std::enable_if<
    std::is_integral<T>::value && std::is_integral<U>::value,
    bool
>::type
constexpr inline equal(const T & lhs, const U & rhs) {
    return safe_compare_detail::equal<
        std::numeric_limits<T>::is_signed,
        std::numeric_limits<U>::is_signed
    >:: invoke(lhs, rhs);
}

template<class T, class U>
typename std::enable_if<
    std::is_floating_point<T>::value && std::is_floating_point<U>::value,
    bool
>::type
constexpr inline equal(const T & lhs, const U & rhs) {
    return lhs == rhs;
}

template<class T, class U>
constexpr inline bool not_equal(const T & lhs, const U & rhs) {
    return ! equal(lhs, rhs);
}

} // safe_compare
} // safe_numerics
} // boost

#endif // BOOST_NUMERIC_SAFE_COMPARE_HPP
