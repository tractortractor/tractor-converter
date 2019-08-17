#ifndef TRACTOR_CONVERTER_DEFINES_H
#define TRACTOR_CONVERTER_DEFINES_H

#include <string>



namespace tractor_converter{



namespace define{
  const std::string version = "2.0.0";
} // namespace define

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
    const std::string config =                       "tractor_converter.cfg";
    const bool readable_output =                     false;
    const bool reversed =                            false;
    const bool usage_pal_for_each_file =             false;
    const bool pal_for_each_file =                   false;
    const bool items_bmp =                           false;
    const bool fix_null_bytes_and_direction =        false;
    const unsigned int obj_float_precision =         6;
    const double default_scale =                     0.195313;
    const std::string m3d_weapon_file =              "";
    const std::string weapon_attachment_point_file = "";
    const std::string ghost_wheel_file =             "";
    const bool extract_bound_model =                 false;
    const bool extract_center_of_mass =              false;
    const std::string center_of_mass_file =          "";
    const std::string wavefront_mtl =                "";
    const bool extract_nonexistent_weapons =         false;
    const bool use_custom_volume_by_default =        false;
    const bool use_custom_rcm_by_default =           false;
    const bool use_custom_J_by_default =             false;
    const std::string default_c3d_material =         "body_red";
    const double scale_cap =                         0.560547;
    const bool center_model =                        false;
    const bool recalculate_vertex_normals =          false;
    const std::string max_smooth_angle =             "30d";
    const bool gen_bound_models =                    false;
    const std::size_t gen_bound_layers_num =         100;
    const double gen_bound_area_threshold =          0.25;
    const std::size_t mtl_n_wheels =                 10;
  } // namespace default_val

  namespace max{
    const double gen_bound_area_threshold = 0.7;
    const std::string gen_bound_area_threshold_str =
      std::to_string(gen_bound_area_threshold);
  } // namespace max



  namespace per_file{

    namespace name{
      const std::string overwrite_volume_main = "overwrite_volume_main";
      const std::string custom_center_of_mass_main =
        "custom_center_of_mass_main";
      const std::string overwrite_inertia_tensor_main =
        "overwrite_inertia_tensor_main";
      const std::string custom_volume_main = "custom_volume_main";
      const std::string custom_inertia_tensor_main =
        "custom_inertia_tensor_main";

      const std::string overwrite_volume_debris =
        "overwrite_volume_debris";
      const std::string custom_center_of_mass_debris =
        "custom_center_of_mass_debris";
      const std::string overwrite_inertia_tensor_debris =
        "overwrite_inertia_tensor_debris";
      const std::string custom_volume_debris = "custom_volume_debris";
      const std::string custom_inertia_tensor_debris =
        "custom_inertia_tensor_debris";

      const std::string overwrite_volume_animated =
        "overwrite_volume_animated";
      const std::string custom_center_of_mass_animated =
        "custom_center_of_mass_animated";
      const std::string overwrite_inertia_tensor_animated =
        "overwrite_inertia_tensor_animated";
      const std::string custom_volume_animated = "custom_volume_animated";
      const std::string custom_inertia_tensor_animated =
        "custom_inertia_tensor_animated";
    } // namespace name

    namespace default_val{
      const bool overwrite_volume_main = false;
      const bool custom_center_of_mass_main = false;
      const bool overwrite_inertia_tensor_main = false;

      const bool overwrite_volume_debris = false;
      const bool custom_center_of_mass_debris = false;
      const bool overwrite_inertia_tensor_debris = false;

      const bool overwrite_volume_animated = false;
      const bool custom_center_of_mass_animated = false;
      const bool overwrite_inertia_tensor_animated = false;
    } // namespace default_val

  } // namespace per_file

} // namespace option

namespace mode{
  namespace name{
    const std::string usage_pal =                 "usage_pal";
    const std::string remove_not_used_pal =       "remove_not_used_pal";
    const std::string tga_merge_unused_pal =      "tga_merge_unused_pal";
    const std::string tga_replace_pal =           "tga_replace_pal";
    const std::string extract_tga_pal =           "extract_tga_pal";
    const std::string vangers_pal_to_tga_pal =    "vangers_pal_to_tga_pal";
    const std::string pal_shift_for_vangers_avi = "pal_shift_for_vangers_avi";
    const std::string cmp_bmp_escave_outside =    "compare_bmp_escave_outside";
    const std::string bmp_to_tga =                "bmp_to_tga";
    const std::string tga_to_bmp =                "tga_to_bmp";
    const std::string vangers_3d_model_to_obj =   "vangers_3d_model_to_obj";
    const std::string obj_to_vangers_3d_model =   "obj_to_vangers_3d_model";
    const std::string create_wavefront_mtl =      "create_wavefront_mtl";
    const std::string create_materials_table =    "create_materials_table";
  } // namespace name
} // namespace mode

namespace folder{
  const std::string resource = "resource";
    const std::string m3d =      "m3d";
      const std::string ammun =    "ammun";
      const std::string animated = "animated";
      const std::string fauna =    "fauna";
      const std::string items =    "items";
      const std::string mechous =  "mechous";
      const std::string unique =   "unique";
      const std::string weapon =   "weapon";
} // namespace folder

namespace ext{
  const std::string delimiter = ".";
  const std::string m3d =       delimiter + "m3d";
  const std::string a3d =       delimiter + "a3d";
  const std::string prm =       delimiter + "prm";
  const std::string lst =       delimiter + "lst";

  const std::string obj =       delimiter + "obj";
  const std::string mtl =       delimiter + "mtl";

  const std::string tga =       delimiter + "tga";
  const std::string bmp =       delimiter + "bmp";
  const std::string pal =       delimiter + "pal";

  const std::string html =      delimiter + "html";

  namespace readable{
    const std::string prefix =        "*";
    const std::string m3d =           prefix + ext::m3d;
    const std::string a3d =           prefix + ext::a3d;
    const std::string m3d_and_a3d =   m3d + "/" + a3d;
    const std::string prm =           prefix + ext::prm;
    const std::string lst =           prefix + ext::lst;

    const std::string obj =           prefix + ext::obj;
    const std::string wavefront_obj = "Wavefront " + prefix + ext::obj;
    const std::string mtl =           prefix + ext::mtl;

    const std::string tga =           prefix + ext::tga;
    const std::string bmp =           prefix + ext::bmp;
    const std::string pal =           prefix + ext::pal;

    const std::string html =          prefix + ext::html;
  } // namespace readable
} // namespace ext

namespace file{
  const std::string game_lst =    "game" + ext::lst;
  const std::string default_prm = "default" + ext::prm;
} // namespace file

// For helpers.
enum class error_handling{none, throw_exception};



} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_DEFINES_H
