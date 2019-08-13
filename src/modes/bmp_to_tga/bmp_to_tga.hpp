#ifndef TRACTOR_CONVERTER_BMP_TO_TGA_H
#define TRACTOR_CONVERTER_BMP_TO_TGA_H

#include "defines.hpp"
#include "tga_constants.hpp"

#include "hex.hpp"
#include "check_option.hpp"
#include "file_operations.hpp"

#include <boost/program_options.hpp>

#include <exception>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>
#include <map>



namespace tractor_converter{



void bmp_to_tga_mode(const boost::program_options::variables_map options);



} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_BMP_TO_TGA_H
