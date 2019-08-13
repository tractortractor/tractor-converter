#ifndef TRACTOR_CONVERTER_PARSE_MTL_BODY_OFFS_H
#define TRACTOR_CONVERTER_PARSE_MTL_BODY_OFFS_H

#include "defines.hpp"
#include "vangers_3d_model_constants.hpp"

#include <exception>
#include <stdexcept>

#include <string>
#include <vector>



namespace tractor_converter{
namespace helpers{



mat_tables::offset_map parse_mtl_body_offs(
  const std::vector<std::string> &body_offsets_str);



} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_PARSE_MTL_BODY_OFFS_H
