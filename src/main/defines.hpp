#ifndef TRACTOR_CONVERTER_DEFINES_H
#define TRACTOR_CONVERTER_DEFINES_H

#include <string>

namespace tractor_converter{

namespace define{
  const std::string version = "1.0.0";
} // namespace defines

// For program options.
namespace option{

  namespace name{
    const std::string obj_float_precision = "3d_obj_float_precision";
    const std::string default_scale = "3d_default_scale";
    const std::string default_c3d_material = "default_c3d_material";
    const std::string scale_cap = "3d_scale_cap";
    const std::string max_smooth_angle = "max_smooth_angle";
    const std::string gen_bound_layers_num = "generate_bound_layers_num";
    const std::string gen_bound_area_threshold =
      "generate_bound_area_threshold";
    const std::string mtl_n_wheels = "mtl_n_wheels";
  } // namespace name

  namespace default_val{
    const int obj_float_precision = 6;
    const std::string obj_float_precision_str =
      std::to_string(obj_float_precision);

    const double default_scale = 0.195313;
    const std::string default_scale_str = std::to_string(default_scale);

    const std::string default_c3d_material = "body_red";

    const double scale_cap = 0.560547;
    const std::string scale_cap_str =
      std::to_string(scale_cap);

    const std::string max_smooth_angle = "30d";

    const std::size_t gen_bound_layers_num = 100;
    const std::string gen_bound_layers_num_str =
      std::to_string(gen_bound_layers_num);

    const double gen_bound_area_threshold = 0.25;
    const std::string gen_bound_area_threshold_str =
      std::to_string(gen_bound_area_threshold);

    const std::size_t mtl_n_wheels = 50;
    const std::string mtl_n_wheels_str = std::to_string(mtl_n_wheels);
  } // namespace default_val

  namespace max{
    const double gen_bound_area_threshold = 0.7;
    const std::string gen_bound_area_threshold_str =
      std::to_string(gen_bound_area_threshold);
  } // namespace max

} // namespace option

// For helpers.
enum class error_handling{none, throw_exception};

} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_DEFINES_H

