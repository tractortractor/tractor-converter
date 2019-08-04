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
    const std::string version = "version";
    const std::string help = "help";
    const std::string config = "config";
    const std::string mode = "mode";
    const std::string source_dir = "source_dir";
    const std::string source_file = "source_file";
    const std::string output_dir = "output_dir";
    const std::string output_file = "output_file";
    const std::string readable_output = "readable_output";
    const std::string reversed = "reversed";
    const std::string usage_pal_for_each_file = "usage_pal_for_each_file";
    const std::string output_dir_unused = "output_dir_unused";
    const std::string usage_pals_dir = "usage_pals_dir";
    const std::string unused_pals_dir = "unused_pals_dir";
    const std::string pal_for_each_file = "pal_for_each_file";
    const std::string pal = "pal";
    const std::string pal_dir = "pal_dir";
    const std::string dir_to_compare = "dir_to_compare";
    const std::string items_bmp = "items_bmp";
    const std::string map = "map";
    const std::string output_dir_through_map = "output_dir_through_map";
    const std::string fix_null_bytes_and_direction =
      "fix_null_bytes_and_direction";
    const std::string obj_float_precision = "3d_obj_float_precision";
    const std::string default_scale = "default_scale";
    const std::string m3d_weapon_file = "m3d_weapon_file";
    const std::string weapon_attachment_point_file =
      "weapon_attachment_point_file";
    const std::string ghost_wheel_file = "ghost_wheel_file";
    const std::string extract_bound_model = "extract_bound_model";
    const std::string extract_center_of_mass = "extract_center_of_mass";
    const std::string center_of_mass_file = "center_of_mass_file";
    const std::string wavefront_mtl = "wavefront_mtl";
    const std::string extract_nonexistent_weapons =
      "extract_nonexistent_weapons";
    const std::string use_custom_volume_by_default =
      "use_custom_volume_by_default";
    const std::string use_custom_rcm_by_default = "use_custom_rcm_by_default";
    const std::string use_custom_J_by_default = "use_custom_J_by_default";
    const std::string default_c3d_material = "default_c3d_material";
    const std::string scale_cap = "3d_scale_cap";
    const std::string center_model = "center_model";
    const std::string recalculate_vertex_normals =
      "recalculate_vertex_normals";
    const std::string max_smooth_angle = "max_smooth_angle";
    const std::string gen_bound_models = "generate_bound_models";
    const std::string gen_bound_layers_num = "generate_bound_layers_num";
    const std::string gen_bound_area_threshold =
      "generate_bound_area_threshold";
    const std::string mtl_n_wheels = "mtl_n_wheels";
    const std::string mtl_body_offs = "mtl_body_offs";
  } // namespace name

  namespace default_val{
    const std::string config = "tractor_converter.cfg";

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

