//
// Created by alex on 11/11/21.
//

#ifndef TURTLE_TURTLE_H
#define TURTLE_TURTLE_H

#include <boost/filesystem.hpp>

#include "boost/program_options.hpp"
#include "global.hpp"

boost::program_options::variables_map turtle_startup(int argc, char **argv);
int turtle_main(const boost::program_options::variables_map args);

#endif // TURTLE_TURTLE_H
