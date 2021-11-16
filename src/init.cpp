//
// Created by alex on 11/15/21.
//
#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/manipulators/to_log.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include "global.hpp"
#include <iostream>


void init() {
  struct severity_tag;
  std::set_terminate(&terminate_handler);
  //%Severity%: %Message%
  const auto &ft = boost::log::keywords::format =
      boost::log::expressions::stream
      << boost::log::expressions::attr<boost::log::trivial::severity_level,
                                       severity_tag>("Severity")
      << ": " << boost::log::expressions::smessage;
  boost::log::add_console_log(std::clog, ft);
}
