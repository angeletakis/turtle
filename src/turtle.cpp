#include "turtle.hpp"

#include "boost/program_options.hpp"
#include "global.hpp"
#include "lexicalize.hpp"
#include "node.hpp"
#include "tokenize.hpp"
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

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

boost::program_options::variables_map turtle_startup(int argc, char **argv) {
  namespace po = boost::program_options;
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
    std::exit(0);
  }
  return vm;
}
