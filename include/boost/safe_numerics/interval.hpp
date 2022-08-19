#ifndef BOOST_SAFE_NUMERICS_INTERVAL_HPP
#define BOOST_SAFE_NUMERICS_INTERVAL_HPP

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>  // minmax, min, max
#include <boost/logic/tribool.hpp>
#include <cassert>
#include <initializer_list>
#include <limits>
#include <type_traits>

#include "utility.hpp"  // log

// from stack overflow
// http://stackoverflow.com/questions/23815138/implementing-variadic-min-max-functions

namespace boost {
namespace safe_numerics {

// # Description
// A closed arithmetic interval represented by a pair of elements of type R. In
// principle, one should be able to use Boost.Interval library for this. But the
// functions in this library are not constexpr. Also, this Boost.Interval is
// more complex and does not support certain operations such bit operations.
// Perhaps some time in the future, Boost.Interval will be used instead of this
// interval<R> type.
//
// # Template Parameters
// R must model the type requirements of Numeric. Note this in principle
// includes any numeric type including floating point numbers and instances of
// checked_result<R>.
//
// # Notation
// - I	An interval type
// - i, j	An instance of interval type
// - R	Numeric types which can be used to make an interval
// - r	An instance of type R
// - p	An instance of std::pair<R, R>
// - l, u	Lowermost and uppermost values in an interval
// - os	std::basic_ostream<class CharT, class Traits = std::char_traits<CharT>>
//
// # Associated Types
// checked_result	holds either the result of an operation or information
// as to why it failed
//
// # Valid Expressions
template <typename R>
struct interval {
  // 下界
  // Expression: i.l
  // ReturnType: R
  // Semantics: lowermost value in the interval i
  const R l;

  // 上界
  // Expression: i.u
  // ReturnType: R
  // Semantics: uppermost value in the interval i
  const R u;

  // Expression: interval<R>(l, u)
  // ReturnType: interval<R>
  // Semantics: construct a new interval from a pair of limits
  template <typename T>
  constexpr interval(const T &lower, const T &upper) : l(lower), u(upper) {
    // assert(static_cast<bool>(l <= u));
  }

  // Expression: interval<R>(p)
  // ReturnType: interval<R>
  // Semantics: construct a new interval from a pair of limits
  template <typename T>
  constexpr interval(const std::pair<T, T> &p) : l(p.first), u(p.second) {}

  // Expression: interval<R>(i)
  // ReturnType: interval<R>
  // Semantics: copy constructor
  template <class T>
  constexpr interval(const interval<T> &rhs) : l(rhs.l), u(rhs.u) {}

  // Expression: make_interval<R>()
  // ReturnType: interval<R>
  // Semantics: return new interval with std::numric_limits<R>::min() and
  // std::numric_limits<R>::max()
  constexpr interval()
      : l(std::numeric_limits<R>::min()), u(std::numeric_limits<R>::max()) {}

  // return true if this interval contains the given point
  constexpr tribool includes(const R &t) const { return l <= t && t <= u; }

  // if this interval contains every point found in some other inteval t
  //  return true
  // otherwise
  //  return false or indeterminate
  constexpr tribool includes(const interval<R> &t) const {
    return u >= t.u && l <= t.l;
  }

  // return true if this interval contains the given point
  constexpr tribool excludes(const R &t) const { return t < l || t > u; }
  // if this interval excludes every point found in some other inteval t
  //  return true
  // otherwise
  //  return false or indeterminate
  constexpr tribool excludes(const interval<R> &t) const {
    return t.u < l || u < t.l;
  }
};

// Expression: make_interval<R>()
// ReturnType: interval<R>
// Semantics: return new interval with std::numric_limits<R>::min() and
// std::numric_limits<R>::max()
template <class R>
constexpr inline interval<R> make_interval() {
  return interval<R>();
}

template <class R>
constexpr inline interval<R> make_interval(const R &) {
  return interval<R>();
}

// account for the fact that for floats and doubles
// the most negative value is called "lowest" rather
// than min
template <>
constexpr inline interval<float>::interval()
    : l(std::numeric_limits<float>::lowest()),
      u(std::numeric_limits<float>::max()) {}
template <>
constexpr inline interval<double>::interval()
    : l(std::numeric_limits<double>::lowest()),
      u(std::numeric_limits<double>::max()) {}

// Expression: i + j
// Return Type: interval<R>
// add two intervals and return the result
template <typename T>
constexpr inline interval<T> operator+(const interval<T> &t,
                                       const interval<T> &u) {
  // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
  return {t.l + u.l, t.u + u.u};
}

// Expression: i - j	
// Return Type: interval<R>	
// Semantic: subtract two intervals and return the result
template <typename T>
constexpr inline interval<T> operator-(const interval<T> &t,
                                       const interval<T> &u) {
  // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
  return {t.l - u.u, t.u - u.l};
}

// Expression: i * j	
// Return Type: interval<R>	
// Semantics: multiply two intervals and return the result
template <typename T>
constexpr inline interval<T> operator*(const interval<T> &t,
                                       const interval<T> &u) {
  // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
  return utility::minmax<T>(
      std::initializer_list<T>{t.l * u.l, t.l * u.u, t.u * u.l, t.u * u.u});
}

// interval division
// note: presumes 0 is not included in the range of the denominator
template <typename T>
constexpr inline interval<T> operator/(const interval<T> &t,
                                       const interval<T> &u) {
  assert(static_cast<bool>(u.excludes(T(0))));
  return utility::minmax<T>(
      std::initializer_list<T>{t.l / u.l, t.l / u.u, t.u / u.l, t.u / u.u});
}

// modulus of two intervals.  This will give a new range of for the modulus.
// note: presumes 0 is not included in the range of the denominator
template <typename T>
constexpr inline interval<T> operator%(const interval<T> &t,
                                       const interval<T> &u) {
  assert(static_cast<bool>(u.excludes(T(0))));
  return utility::minmax<T>(
      std::initializer_list<T>{t.l % u.l, t.l % u.u, t.u % u.l, t.u % u.u});
}

template <typename T>
constexpr inline interval<T> operator<<(const interval<T> &t,
                                        const interval<T> &u) {
  // return interval<T>{t.l << u.l, t.u << u.u};
  return utility::minmax<T>(
      std::initializer_list<T>{t.l << u.l, t.l << u.u, t.u << u.l, t.u << u.u});
}

template <typename T>
constexpr inline interval<T> operator>>(const interval<T> &t,
                                        const interval<T> &u) {
  // return interval<T>{t.l >> u.u, t.u >> u.l};
  return utility::minmax<T>(
      std::initializer_list<T>{t.l >> u.l, t.l >> u.u, t.u >> u.l, t.u >> u.u});
}

// union of two intervals
template <typename T>
constexpr interval<T> operator|(const interval<T> &t, const interval<T> &u) {
  const T &rl = std::min(t.l, u.l);
  const T &ru = std::max(t.u, u.u);
  return interval<T>(rl, ru);
}

// intersection of two intervals
template <typename T>
constexpr inline interval<T> operator&(const interval<T> &t,
                                       const interval<T> &u) {
  const T &rl = std::max(t.l, u.l);
  const T &ru = std::min(t.u, u.u);
  return interval<T>(rl, ru);
}

// determine whether two intervals intersect
template <typename T>
constexpr inline boost::logic::tribool intersect(const interval<T> &t,
                                                 const interval<T> &u) {
  return t.u >= u.l || t.l <= u.u;
}

// Expression: t < u	
// ReturnType: boost::logic::tribool	
// Semantics: true if every element in t is less than every element in u
template <typename T>
constexpr inline boost::logic::tribool operator<(const interval<T> &t,
                                                 const interval<T> &u) {
  return
      // if every element in t is less than every element in u
      t.u < u.l ? boost::logic::tribool(true) :
                // if every element in t is greater than every element in u
          t.l > u.u
          ? boost::logic::tribool(false)
          :
          // otherwise some element(s) in t are greater than some element in u
          boost::logic::indeterminate;
}

// Expression: t > u	
// ReturnType: boost::logic::tribool	
// Semantics: true if every element in t is greater than every element in u
template <typename T>
constexpr inline boost::logic::tribool operator>(const interval<T> &t,
                                                 const interval<T> &u) {
  return
      // if every element in t is greater than every element in u
      t.l > u.u ? boost::logic::tribool(true) :
                // if every element in t is less than every element in u
          t.u < u.l
          ? boost::logic::tribool(false)
          :
          // otherwise some element(s) in t are greater than some element in u
          boost::logic::indeterminate;
}

// Expression: t == u	
// ReturnType: bool	
// Semantics: true if limits are equal
template <typename T>
constexpr inline bool operator==(const interval<T> &t, const interval<T> &u) {
  // intervals have the same limits
  return t.l == u.l && t.u == u.u;
}

// Expression: t != u	
// ReturnType: bool	
// Semantics: true if limits are not equal
template <typename T>
constexpr inline bool operator!=(const interval<T> &t, const interval<T> &u) {
  return !(t == u);
}

// Expression: t <= u	
// ReturnType: boost::logic::tribool	
// Semantics: true if every element in t is less than or equal to every element in u
template <typename T>
constexpr inline boost::logic::tribool operator<=(const interval<T> &t,
                                                  const interval<T> &u) {
  return !(t > u);
}

// Expression: t >= u	
// ReturnType: boost::logic::tribool	
// Semantics: true if every element in t is greater than or equal to every element in u
template <typename T>
constexpr inline boost::logic::tribool operator>=(const interval<T> &t,
                                                  const interval<T> &u) {
  return !(t < u);
}

}  // namespace safe_numerics
}  // namespace boost

#include <iosfwd>

namespace std {

// Expression: os << i	
// ReturnType: os &	
// Semantics: print interval to output stream
template <typename CharT, typename Traits, typename T>
inline std::basic_ostream<CharT, Traits> &operator<<(
    std::basic_ostream<CharT, Traits> &os,
    const boost::safe_numerics::interval<T> &i) {
  return os << '[' << i.l << ',' << i.u << ']';
}

// Expression: os << i	
// ReturnType: os &	
// Semantics: print interval to output stream
// 对unsigned char类型进行特化, 依旧以数值形式输出
template <typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(
    std::basic_ostream<CharT, Traits> &os,
    const boost::safe_numerics::interval<unsigned char> &i) {
  os << "[" << (unsigned)i.l << "," << (unsigned)i.u << "]";
  return os;
}

// Expression: os << i	
// ReturnType: os &	
// Semantics: print interval to output stream
// 对signed char类型进行特化, 依旧以数值形式输出
template <typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(
    std::basic_ostream<CharT, Traits> &os,
    const boost::safe_numerics::interval<signed char> &i) {
  os << "[" << (int)i.l << "," << (int)i.u << "]";
  return os;
}

}  // namespace std

#endif  // BOOST_SAFE_NUMERICS_INTERVAL_HPP
