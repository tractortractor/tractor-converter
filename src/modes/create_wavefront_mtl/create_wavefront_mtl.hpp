#ifndef TRACTOR_CONVERTER_CREATE_WAVEFRONT_MTL_H
#define TRACTOR_CONVERTER_CREATE_WAVEFRONT_MTL_H

#include "defines.hpp"
#include "tga_constants.hpp"
#include "wavefront_obj_constants.hpp"
#include "vangers_3d_model_constants.hpp"

#include "hex.hpp"
#include "check_option.hpp"
#include "file_operations.hpp"
#include "to_string_precision.hpp"

#include "alphanum.hpp"

#include <boost/program_options.hpp>

#include <exception>
#include <stdexcept>

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

namespace tractor_converter{

const int create_wavefront_mtl_max_end_offset = 254;

struct mtl_color
{
  double r;
  double g;
  double b;

  double &operator[](std::size_t to_retrieve);
  const double &operator[](std::size_t to_retrieve) const;
};

typedef std::map<std::string, mtl_color, doj::alphanum_less<std::string>>
  mtl_color_map;

mtl_color create_wavefront_mtl_mode_helper_read_color(
  const std::string &data,
  const c3d::color::offset_pair &offset_pair);

void create_wavefront_mtl_mode_helper_read_colors(
  const std::string &data,
  const c3d::color::offset_map &offset_pair_map,
  mtl_color_map &colors_map);

void create_wavefront_mtl_mode_helper_write_colors(
  const mtl_color_map &colors_map,
  std::string precision_mtl_str,
  std::string &data);

void create_wavefront_mtl_mode(
  const boost::program_options::variables_map options);

} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_CREATE_WAVEFRONT_MTL_H
