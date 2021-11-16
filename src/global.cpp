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

void panic(
    std::string msg,
    const std::experimental::source_location& location,
    const boost::stacktrace::stacktrace &st) {
  LOG(error) << msg << '\n'
             << location.file_name() << ":" << location.line() << ":"
             << location.column() << " `" << location.function_name() << "`\n"
             << st;
  exit(1);
}
