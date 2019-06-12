#ifndef TRACTOR_CONVERTER_CREATE_MATERIALS_TABLE_H
#define TRACTOR_CONVERTER_CREATE_MATERIALS_TABLE_H

#include "defines.hpp"
#include "tga_constants.hpp"
#include "vangers_3d_model_constants.hpp"

#include "hex.hpp"
#include "check_option.hpp"
#include "get_option.hpp"
#include "parse_mtl_body_offs.hpp"
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
#include <utility>

namespace tractor_converter{

enum class html_material_changes{none, per_world, per_quant};

const std::pair<int,int> color_ind_changes_per_world_range = {0 , 127};
const std::pair<int,int> color_ind_changes_per_quant_range = {88, 119};

const int create_materials_table_max_end_offset = 254;

const int create_materials_table_max_colors_per_mat = 124;

const std::string html_table_header =
  "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
  "<table>\n"
  "<tr>\n"
  "  <th>material</th>\n"
  "  <th>offset</th>\n"
  "  <th>shift</th>\n"
  "  <th>Changes</th>\n"
  "  <th rowspan=\"" +
       std::to_string(create_materials_table_max_colors_per_mat) +
       "\">colors</th>\n"
  "</tr>\n";
const std::string html_table_footer =
  "</table>\n";

const std::string html_table_changes_none =      "never";
const std::string html_table_changes_per_world = "each world or cycle";
const std::string html_table_changes_per_quant = "each second";



const std::size_t html_table_header_size = html_table_header.size();
const std::size_t html_table_footer_size = html_table_footer.size();

const std::size_t expected_size_per_material =
  // "  <tr>" + 2 newline
  6 + 2 +
  // "    <td>body_offset_144_shift_3</td>" + 2 newline
  36 + 2 +
  // "    <td>255</td>" + 2 newline // 255 - max offset
  16 + 2 +
  // "    <td>7</td>" + 2 newline // 7 - max shift
  14 + 2 +
  // "    <td>" + html_table_changes_per_world + "</td>" + 2 newline
  8 +
//    html_table_changes_per_world.size()
    std::max({html_table_changes_none.size(),
              html_table_changes_per_world.size(),
              html_table_changes_per_quant.size()}) +
    5 + 2 +
  // "  </tr>" + 2 newline
  7 + 2;

const std::size_t expected_size_per_material_color =
  // "    <td><div style=\"background-color:#99ff00; "
  //   "height:2ex; width:2ex\"></div></td>" + 2 newline
  46 + 43 + 2;



struct html_color
{
  unsigned char r;
  unsigned char g;
  unsigned char b;

  unsigned char &operator[](std::size_t to_retrieve);
  const unsigned char &operator[](std::size_t to_retrieve) const;
};



struct html_material : public c3d::color::offset_pair
{
  html_material();

  html_material(std::vector<html_color> colors_arg,
                int offset_arg,
                int shift_arg,
                html_material_changes changes_arg);

  html_material(std::vector<html_color> colors_arg,
                c3d::color::offset_pair pair_arg,
                html_material_changes changes_arg);

  std::vector<html_color> colors;

  html_material_changes changes;
};



typedef std::map<std::string, html_material, doj::alphanum_less<std::string>>
  html_material_map;

html_material create_materials_table_mode_helper_read_material(
  const std::string &data,
  const c3d::color::offset_pair &offset_pair);

void create_materials_table_mode_helper_read_materials(
  const std::string &data,
  const c3d::color::offset_map &offset_pair_map,
  html_material_map &materials_map);

void create_materials_table_mode_helper_write_materials(
  const html_material_map &materials_map,
  std::string &data);

void create_materials_table_mode(
  const boost::program_options::variables_map options);

} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_CREATE_MATERIALS_TABLE_H
