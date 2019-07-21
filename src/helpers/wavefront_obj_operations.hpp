#ifndef TRACTOR_CONVERTER_WAVEFRONT_OBJ_OPERATIONS_H
#define TRACTOR_CONVERTER_WAVEFRONT_OBJ_OPERATIONS_H

#include "defines.hpp"
#include "wavefront_obj_constants.hpp"
#include "vangers_3d_model_constants.hpp"

//#include "raw_num_operations.hpp"
#include "file_operations.hpp"
#include "to_string_precision.hpp"
//#include "vangers_cfg_operations.hpp"
#include "vangers_3d_model_operations.hpp"

#include "volInt.hpp"
#include "tiny_obj_loader.h"

#include <boost/filesystem.hpp>
//#include <boost/filesystem/fstream.hpp>

#include <exception>
#include <stdexcept>

#include <cstring>
//#include <cmath>
//#include <iostream>
#include <algorithm>
#include <utility>
//#include <iterator>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

namespace tractor_converter{
namespace helpers{

const std::size_t float_precision_objs_default = 6;

const std::string float_precision_objs_string_default =
  "%." + std::to_string(float_precision_objs_default) + "f";

// 1 "v" +
// 3 "num of coords" *
// (1 space + 1 sign + 5 digits left to dot + 1 dot +
//    float_precision_objs digits right to dot) +
// 2 newline
const std::size_t expected_vertex_size_default =
  1 +
  volInt::axes_num * (1 + 1 + 5 + 1 + float_precision_objs_default) +
  2;

// +1 because "vn" instead of "v"
const std::size_t expected_normal_size_default =
  1 + expected_vertex_size_default;

// 50 material string size + 2 newline +
// 1 "f" +
// 4 "num of vertices" *
//   (1 space + 19 digits for vertex + 2 "//" + 19 digits for normal) +
// 2 newline
const std::size_t expected_polygon_size = 50 + 2 +
                                          1 +
                                          4 * (1 + 19 + 2 + 19) +
                                          2;



int raw_obj_to_volInt_helper_get_wheel_weapon_id(
  std::string mat_name,
  std::size_t mat_size);

void raw_obj_to_volInt_helper_get_body_color(
  const boost::filesystem::path &input_file_path_arg,
  const std::string &input_file_name_error,
  std::string mat_name,
  volInt::polyhedron &volInt_model);
void raw_obj_to_volInt_helper_get_wheel_properties(
  const boost::filesystem::path &input_file_path_arg,
  const std::string &input_file_name_error,
  std::size_t mat,
  std::string mat_name,
  std::vector<int> &tiny_obj_id_to_wheel_weapon_id,
  volInt::polyhedron &volInt_model);
void raw_obj_to_volInt_helper_get_weapon_num(
  const boost::filesystem::path &input_file_path_arg,
  const std::string &input_file_name_error,
  std::size_t mat,
  std::string mat_name,
  std::vector<int> &tiny_obj_id_to_wheel_weapon_id);
void raw_obj_to_volInt_helper_get_attachment_point_num(
  const boost::filesystem::path &input_file_path_arg,
  const std::string &input_file_name_error,
  std::size_t mat,
  std::string mat_name,
  std::vector<int> &tiny_obj_id_to_wheel_weapon_id);

volInt::polyhedron raw_obj_to_volInt_model(
  const boost::filesystem::path &input_file_path_arg,
  const std::string &input_file_name_error,
  c3d::c3d_type type,
  unsigned int default_color_id);

void save_volInt_as_wavefront_obj(
  const std::unordered_map<std::string, volInt::polyhedron> &c3d_models,
  const boost::filesystem::path &output_path,
  const std::string &output_file_name_error,
  const std::string &float_precision_objs_string =
    float_precision_objs_string_default,
  std::size_t expected_medium_vertex_size = expected_vertex_size_default,
  std::size_t expected_medium_normal_size = expected_normal_size_default);

} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_WAVEFRONT_OBJ_OPERATIONS_H
