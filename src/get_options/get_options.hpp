#ifndef TRACTOR_CONVERTER_GET_OPTIONS_H
#define TRACTOR_CONVERTER_GET_OPTIONS_H

#include "defines.hpp"

#include "check_option.hpp"
#include "vangers_3d_model_constants.hpp"

#include <boost/program_options.hpp>

#include <exception>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>



namespace tractor_converter{

//const double default_3d_default_scale = 0.195313;

boost::program_options::variables_map get_options(int ac, char** av);

} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_GET_OPTIONS_H
