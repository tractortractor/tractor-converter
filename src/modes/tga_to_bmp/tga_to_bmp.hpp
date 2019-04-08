#ifndef TRACTOR_CONVERTER_TGA_TO_BMP_H
#define TRACTOR_CONVERTER_TGA_TO_BMP_H

#include "defines.hpp"
#include "tga_constants.hpp"

#include "hex.hpp"
#include "check_option.hpp"
#include "file_operations.hpp"
#include "tga_class.hpp"
#include "check_pal_color_used.hpp"

#include <boost/program_options.hpp>

#include <exception>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace tractor_converter{

void tga_to_bmp_mode(const boost::program_options::variables_map options);

} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_TGA_TO_BMP_H
