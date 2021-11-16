#include "turtle.hpp"

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

#include "boost/program_options.hpp"
#include "global.hpp"
#include "lexicalize.hpp"
#include "tokenize.h"
#include "node.hpp"

int turtle_main(const boost::program_options::variables_map args) {
  const boost::filesystem::path file = args["file"].as<std::string>();
  if (!boost::filesystem::is_regular_file(file)) {
    PANIC_MACRO("File " << file << " not found");
  }
  turtle::document_t Document;
  boost::filesystem::load_string_file(file, Document.filedata);
  lexicalize(Document.filedata, Document.tokens);
  tokenize(Document.tokens);
  return 0;
}
