#ifndef TRACTOR_CONVERTER_TGA_MERGE_UNUSED_PAL_H
#define TRACTOR_CONVERTER_TGA_MERGE_UNUSED_PAL_H

#include "defines.hpp"
#include "tga_constants.hpp"

#include "hex.hpp"
#include "check_option.hpp"
#include "file_operations.hpp"
#include "tga_class.hpp"
#include "check_pal_color_used.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <exception>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>



namespace tractor_converter{



void tga_merge_unused_pal_mode(
  const boost::program_options::variables_map options);



} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_TGA_MERGE_UNUSED_PAL_H
