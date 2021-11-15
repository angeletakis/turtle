//
// Created by alex on 11/11/21.
//

#include "global.hpp"
#include <boost/stacktrace.hpp>
#include <iostream>

void terminate_handler(void) {
  try {
    std::cerr << boost::stacktrace::stacktrace();
  } catch (...) {
  }
  std::abort();
}
