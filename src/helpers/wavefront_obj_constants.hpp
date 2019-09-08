#ifndef TRACTOR_CONVERTER_WAVEFRONT_OBJ_CONSTANTS_H
#define TRACTOR_CONVERTER_WAVEFRONT_OBJ_CONSTANTS_H

#include "defines.hpp"

#include "vangers_3d_model_constants.hpp"

#include <string>



namespace tractor_converter{
namespace wavefront_obj{



  const std::string mat_separator = "_";
  const std::size_t mat_separator_size = mat_separator.size();

  namespace prefix{
    const std::string main = "main";
    const std::string main_bound = "main_bound";
    const std::string debris = "debris";
    const std::string debris_bound = "debris_bound";
    const std::string animated = "";
  } // namespace prefix

  namespace obj_name{
    const std::string main = "main";
    const std::string weapon = "weapon";
    const std::string attachment_point = "attachment_point";
    const std::string center_of_mass = "center_of_mass";
  } // namespace obj_name

  const double vector_scale_val = 1.0;


  // 2 "o " + 2 newline + 2 "g " + 2 newline
  const std::size_t base_obj_name_size = 2 + 2 + 2 + 2;

  const std::string mtl_filename = "object_materials" + ext::mtl;
  // 19 "mtllib ../../../../" + mtl_filename.size() + 2 '\n'
  const std::size_t mtl_reference_size = 19 + mtl_filename.size() + 2;


  const std::string body_mat_str_el_1 = c3d::color::body::string::el_1;
  const std::size_t body_mat_str_el_1_size = body_mat_str_el_1.size();
  const std::string body_mat_str_el_2 = c3d::color::body::string::el_2;
  const std::size_t body_mat_str_el_2_size = body_mat_str_el_2.size();

  const std::string wheel_mat_marker(mat_separator + "wheel");
  const std::size_t wheel_mat_marker_size = wheel_mat_marker.size();
  const std::string wheel_steer_mat_marker(mat_separator + "steer");
  const std::size_t wheel_steer_mat_marker_size =
    wheel_steer_mat_marker.size();
  const std::string wheel_ghost_mat_marker(mat_separator + "ghost");
  const std::size_t wheel_ghost_mat_marker_size =
    wheel_ghost_mat_marker.size();
  const std::string weapon_mat_marker(mat_separator + "weapon");
  const std::size_t weapon_mat_marker_size = weapon_mat_marker.size();



} // namespace wavefront_obj
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_WAVEFRONT_OBJ_CONSTANTS_H
