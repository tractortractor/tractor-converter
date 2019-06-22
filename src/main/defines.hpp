#ifndef TRACTOR_CONVERTER_DEFINES_H
#define TRACTOR_CONVERTER_DEFINES_H

#include <string>

namespace tractor_converter{

namespace defines{

const std::string version = "1.0.0";

// For program options.
const int obj_float_precision = 6;
const std::string obj_float_precision_str =
  std::to_string(obj_float_precision);

const double default_scale = 0.195313;
const std::string default_scale_str =
  std::to_string(default_scale);

const std::string c3d_default_material = "body_red";

const double default_3d_scale_cap = 0.560547;
const std::string default_3d_scale_cap_str =
  std::to_string(default_3d_scale_cap);

const std::string default_max_smooth_angle = "30d";

} // namespace defines

// For helpers.
enum class error_handling{none, throw_exception};

} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_DEFINES_H

