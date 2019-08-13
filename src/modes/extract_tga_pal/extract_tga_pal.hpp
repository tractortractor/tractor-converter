#ifndef TRACTOR_CONVERTER_EXTRACT_TGA_PAL_H
#define TRACTOR_CONVERTER_EXTRACT_TGA_PAL_H

#include "defines.hpp"
#include "tga_constants.hpp"

#include "hex.hpp"
#include "check_option.hpp"
#include "file_operations.hpp"
#include "tga_class.hpp"

#include <boost/program_options.hpp>

#include <exception>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>
#include <map>



namespace tractor_converter{



void extract_tga_pal_mode(const boost::program_options::variables_map options);



} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_EXTRACT_TGA_PAL_H
