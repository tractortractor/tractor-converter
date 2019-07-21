#ifndef TRACTOR_CONVERTER_WAVEFRONT_OBJ_CONSTANTS_H
#define TRACTOR_CONVERTER_WAVEFRONT_OBJ_CONSTANTS_H

#include "defines.hpp"

#include <string>

namespace tractor_converter{
namespace wavefront_obj{

  const std::string mat_separator = "_";
  const std::size_t mat_separator_size = mat_separator.size();

  namespace obj_name{
    const std::string main = "main";
    const std::string weapon = "weapon";
    const std::string attachment_point = "attachment_point";
    const std::string center_of_mass = "center_of_mass";
  } // namespace obj_name

  const double vector_scale_val = 1.0;


  // 2 "g " + 2 newline
  const std::size_t base_obj_name_size = 2 + 2;

  const std::string mtl_filename = "object_materials.mtl";
  // 19 "mtllib ../../../../" + mtl_filename.size() + 2 '\n'
  const std::size_t mtl_reference_size = 19 + mtl_filename.size() + 2;


  const std::string body_mat_str_el_1(
    "body" + mat_separator + "offset" + mat_separator);
  const std::size_t body_mat_str_el_1_size = body_mat_str_el_1.size();
  const std::string body_mat_str_el_2(mat_separator + "shift" + mat_separator);
  const std::size_t body_mat_str_el_2_size = body_mat_str_el_2.size();

//const std::string wheel_steer_mat_str("wheel_steer_");
//const std::size_t wheel_steer_mat_str_size = wheel_steer_mat_str.size();
  const std::string wheel_mat_str("wheel");
  const std::size_t wheel_mat_str_size = wheel_mat_str.size();
  const std::string wheel_steer_mat_str_el(mat_separator + "steer");
  const std::size_t wheel_steer_mat_str_el_size =
    wheel_steer_mat_str_el.size();
  const std::string wheel_ghost_mat_str_el(mat_separator + "ghost");
  const std::size_t wheel_ghost_mat_str_el_size =
    wheel_ghost_mat_str_el.size();
  const std::string weapon_mat_str("weapon");
  const std::size_t weapon_mat_str_size = weapon_mat_str.size();
  const std::string attachment_point_mat_str("attachment_point");
  const std::size_t attachment_point_mat_str_size =
    attachment_point_mat_str.size();

//const int default_float_precision = 6;

// 2 newline + 1 "v" +
// 3 "num of coords" *
//   (1 space +
//    3 digits left to dot +
//    1 dot +
//    float_precision digits right to dot) +
// 5
//const std::size_t expected_medium_vertex_size =
//  2 + 1 + 3 * (1 + 3 + 1 + float_precision) + 5;

// +1 because "vn" instead of "v"
//const std::size_t expected_medium_normal_size =
//  1 + expected_medium_vertex_size;

// 2 newline + 50 material string size + 2 newline + 1 "f" +
// 4 "num of vertices" *
//   (1 space + 3 digits for vertex + 2 "//" + 3 digits for normal) +
// 5
//const std::size_t expected_polygon_size = 50 + 1 + 4 * (1 + 3 + 2 + 3) + 2;

} // namespace wavefront_obj
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_WAVEFRONT_OBJ_CONSTANTS_H
