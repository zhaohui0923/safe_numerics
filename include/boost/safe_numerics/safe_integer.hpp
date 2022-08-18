#ifndef BOOST_NUMERIC_SAFE_INTEGER_HPP
#define BOOST_NUMERIC_SAFE_INTEGER_HPP

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// not actually used here - but needed for integer arithmetic
// so this is a good place to include it
#include "checked_integer.hpp"
#include "checked_result_operations.hpp"
#include "exception_policies.hpp"
#include "native.hpp"
#include "safe_base.hpp"
#include "safe_base_operations.hpp"

// specialization for meta functions with safe<T> argument
namespace boost {
namespace safe_numerics {

// # Description
//
// A safe<T, PP , EP> can be used anywhere a type T can be used. Any expression
// which uses this type is guaranteed to return an arithmetically correct value
// or to trap in some way.
//
// # Notation
//
// T - Underlying type from which a safe type is being derived
//
// # Associated Types
//
// PP -	Promotion Policy. A type which specifies the result type of an
//      expression using safe types.
// EP -	Exception Policy. A type containing members which are called when a
//      correct result cannot be returned
//
// # Template Parameters
//
// T - The underlying type. Currently only built-in integer types are supported
// PP - Optional promotion policy. Default value is boost::numeric::native
// Optional - exception policy. Default value is
//      boost::numeric::default_exception_policy
//
// # Model of
//
//  - Numeric
//  - Integer
//
// # Valid Expressions
//
// Implements all expressions and only those expressions supported by the base
// type T. Note that all these expressions are constexpr. The result type of
// such an expression will be another safe type. The actual type of the result
// of such an expression will depend upon the specific promotion policy template
// parameter.
//
// When a binary operand is applied to two instances of safe<T, PP, EP>one of
// the following must be true:
//
// - The promotion policies of the two operands must be the same or one of them
// must be void
// - The exception policies of the two operands must be the same or one of them
// must be void
//
// If either of the above is not true, a compile error will result.
template <class T, class P = native, class E = default_exception_policy>
using safe = safe_base<T, ::std::numeric_limits<T>::min(),
                       ::std::numeric_limits<T>::max(), P, E>;

}  // namespace safe_numerics
}  // namespace boost

#endif  // BOOST_NUMERIC_SAFE_INTEGER_HPP
