#ifndef TRACTOR_CONVERTER_GET_OPTION_H
#define TRACTOR_CONVERTER_GET_OPTION_H

#include "defines.hpp"
#include "bitflag.hpp"
#include "check_option.hpp"

#include "volInt.hpp"

#include <boost/program_options.hpp>

#include <exception>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>



namespace tractor_converter{
namespace helpers{



std::vector<std::string> get_vec_str_option(
  const boost::program_options::variables_map &options,
  const std::string &option_name,
  const bitflag<error_handling> error_flags = error_handling::throw_exception);
double get_angle_option(
  const boost::program_options::variables_map &options,
  const std::string &option_name,
  const bitflag<error_handling> error_flags = error_handling::throw_exception);



} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_GET_OPTION_H
