#include <boost/filesystem.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/manipulators/to_log.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <iostream>
#include <string_view>

#include "boost/program_options.hpp"
#include "global.hpp"
#include "node.hpp"
#include "turtle.hpp"
#include "init.h"

namespace po = boost::program_options;

int main(int argc = 0, char **argv = nullptr) {
  // Declare the supported options.
  init();
  // clang-format off
  po::options_description desc("Allowed options");
  desc.add_options()
  ("help,h", po::bool_switch(), "produce help message")
  ("file,f", po::value<std::string>()->default_value("main.py"),  "filename to transcompile");
  // clang-format on

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
  } catch (std::exception const &ex) {
    panic(ex.what());
  }
  po::notify(vm);

  if (vm["help"].as<bool>()) {
    boost::filesystem::path program_path{argv[0]};
    auto progam_name = boost::filesystem::basename(program_path);
    LOG(info) << progam_name << " <arg> <filename>";
    LOG(info) << desc << "\n";
    return 0;
  }

  return turtle_main(vm);
}
