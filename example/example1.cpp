//  Copyright (c) 2018 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/safe_numerics/safe_integer.hpp>
#include <iostream>

int main(int, const char *[]) {
  std::cout << "example 1:";
  std::cout << "undetected erroneous expression evaluation" << std::endl;
  std::cout << "Not using safe numerics" << std::endl;
  try {
    std::int8_t x = 127;
    std::int8_t y = 2;
    std::int8_t z;
    // this produces an invalid result !
    z = x + y;
    std::cout << "error NOT detected!" << std::endl;
    std::cout << (int)z << " != " << (int)x << " + " << (int)y << std::endl;
  } catch (const std::exception &) {
    std::cout << "error detected!" << std::endl;
  }
  // solution: replace int with safe<int>
  std::cout << "Using safe numerics" << std::endl;
  try {
    using namespace boost::safe_numerics;
    // x的类型为boost::safe_numerics::safe_base<int8_t, (int8_t)(-128),
    // (int8_t)127, boost::safe_numerics::native,
    // boost::safe_numerics::default_exception_policy>
    // 在为x赋值的时候就已经抛出异常了
    safe<std::int8_t> x = INT_MAX;
    safe<std::int8_t> y = 2;
    safe<std::int8_t> z;
    // rather than producing an invalid result an exception is thrown
    z = x + y;
  } catch (const std::exception &e) {
    // which we can catch here
    std::cout << "error detected:" << e.what() << std::endl;
  }
  return 0;
}
