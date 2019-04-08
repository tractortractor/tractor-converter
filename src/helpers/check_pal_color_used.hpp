#ifndef TRACTOR_CONVERTER_CHECK_PAL_COLOR_USED_H
#define TRACTOR_CONVERTER_CHECK_PAL_COLOR_USED_H

#include "defines.hpp"
#include "tga_constants.hpp"

#include <string>



namespace tractor_converter{
namespace helpers{

bool check_pal_color_used(const std::size_t pos, const std::string &bytes);

} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_CHECK_PAL_COLOR_USED_H
