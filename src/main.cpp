#include <boost/filesystem.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/manipulators/to_log.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <csignal>
#include <iostream>
#include <string_view>

#include "boost/program_options.hpp"
#include "global.hpp"
#include "init.hpp"
#include "node.hpp"
#include "turtle.hpp"

int main(int argc = 0, char **argv = nullptr) {
  // Declare the supported options.
  init();
  const auto vm = turtle_startup(argc, argv);
  return turtle_main(vm);
}
