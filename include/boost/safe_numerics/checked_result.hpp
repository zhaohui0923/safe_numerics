#ifndef BOOST_NUMERIC_CHECKED_RESULT
#define BOOST_NUMERIC_CHECKED_RESULT

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// contains operations for doing checked aritmetic on NATIVE
// C++ types.
#include <cassert>
#include <type_traits>  // is_convertible

#include "exception.hpp"

namespace boost {
namespace safe_numerics {

// # Description
// checked_result is a special kind of variant class designed to hold the result
// of some operation. It can hold either the result of the operation or
// information on why the operation failed to produce a valid result. It is
// similar to other types proposed for and/or included to the C++ standard
// library or Boost such as expected, variant, optional and outcome. In some
// circumstances it may be referred to as a "monad".
//
// - All instances of checked_result<R> are immutable. That is, once
//  constructed, they cannot be altered.
// - There is no default constructor.
// - checked_result<R> is never empty.
// - Binary operations supported by type R are guaranteed to be supported by
//  checked_result<R>.
// - Binary operations can be invoked on a pair of checked_result<R> instances
//  if and only if the underlying type (R) is identical for both instances. They
//  will return a value of type checked_result<R>.
// - Unary operations can be invoked on checked_result<R> instances. They will
//  return a value of type checked_result<R>.
// - Comparison operations will return a boost::logic::tribool.Other binary
//  operations will a value of the same type as the arguments.
//
// Think of checked<R> as an "extended" version of R which can hold all the
// values that R can hold in addition other "special values". For example,
// consider checked<int>.
//
// # Notation
// - R	Underlying type
// - r	An instance of type R
// - c, c1, c2	an instance of checked_result<R>
// - t	an instance of checked_result<T> for some type T not necessarily the
//  same as R
// - e	An instance of type safe_numerics_error
// - msg	An instance of type const char *
// - OS	A type convertible to std::basic_ostream
// - os	An instance of type convertible to std::basic_ostream
//
// # Template Parameters
// R must model the type requirements of Numeric
// R Underlying type
//
// # Model of
// Numeric
//
// # Valid Expressions
// All expressions are constexpr.
template <typename R>
struct checked_result {
  // 错误枚举
  const safe_numerics_error m_e;
  // 内容联合体
  // 在结果正确的情况下，存储结果
  // 在结果错误的情况下，存储错误信息
  // 为了节省空间，使用了联合体
  // 结果是否正确由外部存储并控制
  union contents {
    // 结果值
    R m_r;
    // 错误信息
    char const *const m_msg;
    // contstructors for different types
    // 构造函数
    // r: 结果值
    constexpr contents(const R &r) noexcept : m_r(r) {}
    // 构造函数
    // msg: 错误信息
    constexpr contents(char const *msg) noexcept : m_msg(msg) {}
    // 类型转换函数 - 用于获取结果值
    constexpr operator R() noexcept { return m_r; }
    // 类型转换函数 - 用于获取错误信息
    constexpr operator char const *() noexcept { return m_msg; }
  };

  // 内容(正确结果或错误信息的联合体)
  contents m_contents;

  // don't permit construction without initial value;
  // 禁用了默认构造函数
  checked_result() = delete;
  // 拷贝构造函数使用编译器提供的默认实现
  checked_result(const checked_result &r) = default;
  // 移动构造函数使用编译器提供的默认实现
  checked_result(checked_result &&r) = default;

  // Expression: checked_result(r)
  // Return Type: checked_result<R>
  // Semantics: constructor with valid instance of R
  //
  // Expression: checked_result<R>(t)
  // Return Type: checked_result<R>
  // Semantics: constructor with checked_result<T> where T is not R.
  //    T must be convertible to R
  constexpr /*explicit*/ checked_result(const R &r) noexcept
      : m_e(safe_numerics_error::success), m_contents{r} {}

  // Expression: checked_result(e, msg)
  // Return Type: checked_result<R>
  // Semantics: constructor with error information(错误枚举和错误描述)
  constexpr /*explicit*/ checked_result(const safe_numerics_error &e,
                                        const char *msg = "") noexcept
      : m_e(e), m_contents{msg} {
    // 使用断言运行时检查错误枚举为错误
    // 因为该构造函数只能用于错误的情况
    assert(m_e != safe_numerics_error::success);
  }

  // permit construct from another checked result type
  template <typename T>
  constexpr /*explicit*/ checked_result(const checked_result<T> &t) noexcept
      : m_e(t.m_e) {
    static_assert(std::is_convertible<T, R>::value,
                  "T must be convertible to R");
    if (safe_numerics_error::success == t.m_e)
      m_contents.m_r = t.m_r;
    else
      m_contents.m_msg = t.m_msg;
  }

  // Expression: c.exception()
  // Return Type: bool
  // Semantics: true if checked_result contains an error condition
  constexpr bool exception() const {
    return m_e != safe_numerics_error::success;
  }

  // accesors
  // Expression: static_cast<R>(c)
  // Return Type: R
  // Semantics: extract wrapped value - compile time error if not possible
  constexpr operator R() const noexcept {
    // don't assert here.  Let the library catch these errors
    // assert(! exception());
    return m_contents.m_r;
  }

  // Expression: static_cast<safe_numerics_error>
  // Return Type: safe_numerics_error
  // Semantics: extract wrapped value - may return safe_numerics_error::success
  //    if there is no error
  constexpr operator safe_numerics_error() const noexcept {
    // note that this is a legitimate operation even when
    // the operation was successful - it will return success
    return m_e;
  }

  // Expression: static_cast<const char *>(c)
  // Return Type: const char *
  // Semantics: returns pointer to the included error message
  constexpr operator const char *() const noexcept {
    assert(exception());
    return m_contents.m_msg;
  }

  // disallow assignment
  // 禁用赋值
  checked_result &operator=(const checked_result &) = delete;
};  // checked_result

template <class R>
class make_checked_result {
 public:
  template <safe_numerics_error E>
  constexpr static checked_result<R> invoke(char const *const &m) noexcept {
    return checked_result<R>(E, m);
  }
};

}  // namespace safe_numerics
}  // namespace boost

#endif  // BOOST_NUMERIC_CHECKED_RESULT
