#ifndef TRACTOR_CONVERTER_REMOVE_NOT_USED_PAL_H
#define TRACTOR_CONVERTER_REMOVE_NOT_USED_PAL_H

#include "defines.hpp"
#include "tga_constants.hpp"

#include "hex.hpp"
#include "check_option.hpp"
#include "file_operations.hpp"
#include "check_pal_color_used.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <exception>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>



namespace tractor_converter{



void remove_not_used_pal_mode(
  const boost::program_options::variables_map options);



} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_REMOVE_NOT_USED_PAL_H
