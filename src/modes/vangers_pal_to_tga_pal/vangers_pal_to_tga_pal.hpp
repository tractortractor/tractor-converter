#ifndef TRACTOR_CONVERTER_VANGERS_PAL_TO_TGA_PAL_H
#define TRACTOR_CONVERTER_VANGERS_PAL_TO_TGA_PAL_H

#include "defines.hpp"
#include "tga_constants.hpp"

#include "hex.hpp"
#include "check_option.hpp"
#include "file_operations.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <exception>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>



namespace tractor_converter{



void vangers_pal_to_tga_pal_mode(
  const boost::program_options::variables_map options);



} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_VANGERS_PAL_TO_TGA_PAL_H
