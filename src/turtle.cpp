//
// Created by alex on 11/11/21.
//

#include "turtle.h"

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

#include "boost/program_options.hpp"
#include "global.h"
#include "node.h"
#include "lexicalize.h"

int turtle_main(const boost::program_options::variables_map args) {
  const boost::filesystem::path file = args["file"].as<std::string>();
  if (!boost::filesystem::is_regular_file(file)) {
    LOG(error) << "File " << file << " not found";
    return 1;
  }
  std::string filedata;
  std::vector<turtle::lexeme_t> lexemes;
  boost::filesystem::load_string_file(file, filedata);
  lexicalize(filedata, lexemes);
  return 0;
}
