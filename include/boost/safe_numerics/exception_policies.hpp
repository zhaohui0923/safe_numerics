#ifndef BOOST_NUMERIC_EXCEPTION_POLICIES_HPP
#define BOOST_NUMERIC_EXCEPTION_POLICIES_HPP

//  Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/mp11.hpp>
#include <boost/config.hpp> // BOOST_NO_EXCEPTIONS
#include "exception.hpp"

namespace boost {
namespace safe_numerics {

// # Description
// Create a valid exception policy from 4 function objects. This specifies the
// actions to be taken for different types of invalid results.
//
// # Notation
// - e	instance of a the type safe_numerics_error
// - message	pointer to const char * error message
//
// # Model of
// ExceptionPolicy
//
// # Inherited Valid Expressions
// This class implements all the valid operations from the type requirements
// ExceptionPolicy. Aside from these, there are no other operations implemented.
//
// # Valid Expressions
// Whenever an operation yield an invalid result, one of the following functions
// will be invoked.
template<
    // Function object callable with the expression AE()(e, message)
    // The operation cannot produce valid arithmetic result such as overflows, divide by zero, etc.
    typename AE,
    // Function object callable with the expression IDB()(e,
    // The result depends upon implementation defined behavior according to the C++ standard
    typename IDB,
    // Function object callable with the expression UB()(e, message)
    // The result is undefined by the C++ standard
    typename UB,
    // Function object callable with the expression UV()(e, message)
    // A variable is not initialized
    typename UV
>
struct exception_policy {
    // # Expression
    // EP::on_arithmetic_error(e, message)
    //
    // # Invoke When
    // The operation cannot produce valid arithmetic result such as overflows,
    // divide by zero, etc.
    constexpr static void on_arithmetic_error(const safe_numerics_error &e,
                                            const char *msg) {
        AE()(e, msg);
    }

    // # Expression
    // EP::on_implementation_defined_behavior(e, message)
    //
    // # Invoke When
    // The result depends upon implementation defined behavior according to the
    // C++ standard
    constexpr static void on_implementation_defined_behavior(
        const safe_numerics_error & e,
        const char * msg
    ){
        IDB()(e, msg);
    }

    // # Expression
    // EP::on_undefined_behavior(e, message)
    // 
    // # Invoke When
    // The result is undefined by the C++ standard
    constexpr static void on_undefined_behavior(
        const safe_numerics_error & e,
        const char * msg
    ){
        UB()(e, msg);
    }

    // # Expression
    // EP::on_uninitialized_value(e, message)
    //
    // # Invoke When
    // A variable is not initialized
    constexpr static void on_uninitialized_value(
        const safe_numerics_error & e,
        const char * msg
    ){
        UV()(e, msg);
    }
};

////////////////////////////////////////////////////////////////////////////////
// pre-made error action handers

// ignore any error and just return.
// 用于忽略任何错误并返回。
struct ignore_exception {
    // 默认的构造函数采用编译器的默认实现
    constexpr ignore_exception() = default;
    // 什么都不做
    constexpr void operator () (
        const boost::safe_numerics::safe_numerics_error &,
        const char *
    ){}
};

// emit compile time error if this is invoked.
// 如果调用了这个函数，就会发出编译时错误。
struct trap_exception {
    // 默认的构造函数采用编译器的默认实现
    constexpr trap_exception() = default;
    // error will occur on operator call.
    // hopefully this will display arguments
    // 由于故意的缺少operaotr()的实现
    // 以此来达到编译时错误的目的
};

// If an exceptional condition is detected at runtime throw the exception.
// 如果在运行时检测到异常条件，则抛出异常。
struct throw_exception {
    // 默认构造函数采用编译器的默认实现
    constexpr throw_exception() = default;
    #ifndef BOOST_NO_EXCEPTIONS
    void operator()(
        const safe_numerics_error & e,
        const char * message
    ){
        // 抛出异常
        throw std::system_error(std::error_code(e), message);
    }
    #else
    constexpr trap_exception()(const safe_numerics_error & e, const char * message);
    #endif
};

// given an error code - return the action code which it corresponds to.
constexpr inline safe_numerics_actions
make_safe_numerics_action(const safe_numerics_error & e){
    // we can't use standard algorithms since we want this to be constexpr
    // this brute force solution is simple and pretty fast anyway
    switch(e){
    case safe_numerics_error::negative_overflow_error:
    case safe_numerics_error::underflow_error:
    case safe_numerics_error::range_error:
    case safe_numerics_error::domain_error:
    case safe_numerics_error::positive_overflow_error:
    case safe_numerics_error::precision_overflow_error:
        return safe_numerics_actions::arithmetic_error;

    case safe_numerics_error::negative_value_shift:
    case safe_numerics_error::negative_shift:
    case safe_numerics_error::shift_too_large:
        return safe_numerics_actions::implementation_defined_behavior;

    case safe_numerics_error::uninitialized_value:
        return safe_numerics_actions::uninitialized_value;

    case safe_numerics_error::success:
        return safe_numerics_actions::no_action;
    default:
        assert(false);
    }
    // should never arrive here
    //include to suppress bogus warning
    return safe_numerics_actions::no_action;
}

////////////////////////////////////////////////////////////////////////////////
// pre-made error policy classes

// loose exception
// - throw on arithmetic errors
// - ignore other errors.
// Some applications ignore these issues and still work and we don't
// want to update them.
using loose_exception_policy = exception_policy<
    throw_exception,    // arithmetic error
    ignore_exception,   // implementation defined behavior
    ignore_exception,   // undefined behavior
    ignore_exception     // uninitialized value
>;

// loose trap
// same as above in that it doesn't check for various undefined behaviors
// but traps at compile time for hard arithmetic errors.  This policy
// would be suitable for older embedded systems which depend on
// bit manipulation operations to work.
using loose_trap_policy = exception_policy<
    trap_exception,    // arithmetic error
    ignore_exception,  // implementation defined behavior
    ignore_exception,  // undefined behavior
    ignore_exception   // uninitialized value
>;

// strict exception
// - throw at runtime on any kind of error
// recommended for new code.  Check everything at compile time
// if possible and runtime if necessary.  Trap or Throw as
// appropriate.  Should guarantee code to be portable across
// architectures.
using strict_exception_policy = exception_policy<
    throw_exception,
    throw_exception,
    throw_exception,
    ignore_exception
>;

// strict trap
// Same as above but requires code to be written in such a way as to
// make it impossible for errors to occur.  This naturally will require
// extra coding effort but might be justified for embedded and/or
// safety critical systems.
using strict_trap_policy = exception_policy<
    trap_exception,
    trap_exception,
    trap_exception,
    trap_exception
>;

// default policy
// One would use this first. After experimentation, one might
// replace some actions with ignore_exception
using default_exception_policy = strict_exception_policy;

} // namespace safe_numerics
} // namespace boost

#endif // BOOST_NUMERIC_EXCEPTION_POLICIES_HPP
