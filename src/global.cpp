#include "global.hpp"

#include <boost/stacktrace.hpp>
#include <cstring>
#include <iostream>

void panic(std::string msg, const std::experimental::source_location &location,
           const boost::stacktrace::stacktrace &st) {
  LOG(error) << msg << std::endl
             << location.file_name() << ":" << location.line() << ":" << location.column() << " `"
             << location.function_name() << '`' << std::endl
             << st;
  std::abort();
}
