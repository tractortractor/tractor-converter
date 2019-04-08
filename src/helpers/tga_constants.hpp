#ifndef TRACTOR_CONVERTER_TGA_CONSTANTS_H
#define TRACTOR_CONVERTER_TGA_CONSTANTS_H

#include "defines.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>

namespace tractor_converter{

const std::size_t vangers_bmp_coords_size = 4;
const std::size_t vangers_bmp_coords_pos = 0;



const std::size_t tga_id_length_pos = 0;
const std::size_t tga_color_map_type_pos = 1;
const std::size_t tga_image_type_pos = 2;
const std::size_t tga_color_map_specification_pos = 3;
const std::size_t tga_color_map_first_entry_index_pos = 3;
const std::size_t tga_color_map_length_pos = 5;
const std::size_t tga_color_map_entry_size_pos = 7;
const std::size_t tga_image_specification_x_origin_pos = 8;
const std::size_t tga_image_specification_y_origin_pos = 10;
const std::size_t tga_image_specification_width_pos = 12;
const std::size_t tga_image_specification_height_pos = 14;
const std::size_t tga_image_specification_pixel_depth_pos = 16;
const std::size_t tga_image_specification_image_descriptor_pos = 17;



const std::size_t tga_header_size = 18;
const std::size_t tga_default_colors_num_in_pal = 256;
const std::size_t tga_default_pal_size = 768;
const std::size_t tga_default_pal_pos = 18;
const std::size_t tga_default_color_size = 3;
const std::size_t tga_default_header_and_pal_size = 786;
const std::size_t tga_coords_pos = 12;
const std::size_t tga_coords_size = 4;


const int tga_color_map_type_expected = 1;
const int tga_color_map_type_empty = 0;
const int tga_image_type_expected = 1;
const std::unordered_map<int, std::string> image_types =
  {
    {0 , "no image data is present"},
    {1 , "uncompressed color-mapped image"},
    {2 , "uncompressed true-color image"},
    {3 , "uncompressed black-and-white (grayscale) image"},
    {9 , "runlength encoded color-mapped image"},
    {10, "runlength encoded true-color image"},
    {11, "compressed encoded black-and-white (grayscale) image"},
    {32, "compressed color-mapped data, using Huffman, Delta, and "
         "runlength encoding"},
    {33, "compressed color-mapped data, using Huffman, Delta, "
         "and runlength encoding.  4-pass quadtree-type process"},
  };

const std::string tga_id_length_str("\0", 1);
const std::string tga_color_map_type_str("\x01", 1);
const std::string tga_image_type_str("\x01", 1);



const std::string tga_color_map_first_entry_index("\0\0", 2);
// 256 entries palette.
const std::string tga_color_map_length("\0\x01", 2);
// 24 bits per entry.
const std::string tga_color_map_entry_size("\x18", 1);

const std::string tga_color_map_specification_str =
  tga_color_map_first_entry_index +
  tga_color_map_length +
  tga_color_map_entry_size;



const std::string tga_image_specification_x_origin_str("\0\0", 2);
const std::string tga_image_specification_y_origin_str("\0\0", 2);
// Dummy value. Replaced later.
const std::string tga_image_specification_width_str("\0\0", 2);
// Dummy value. Replaced later.
const std::string tga_image_specification_height_str("\0\0", 2);
// 8 bits per pixel.
const std::string tga_image_specification_pixel_depth_str("\x08", 1);
// Needed for right angle.
const std::string tga_image_specification_image_descriptor_str("\x20", 1);

const std::string tga_image_specification_str =
  tga_image_specification_x_origin_str +
  tga_image_specification_y_origin_str +
  tga_image_specification_width_str +
  tga_image_specification_height_str +
  tga_image_specification_pixel_depth_str +
  tga_image_specification_image_descriptor_str;



const std::string tga_header_str =
  tga_id_length_str +
  tga_color_map_type_str +
  tga_image_type_str +
  tga_color_map_specification_str +
  tga_image_specification_str;

} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_TGA_CONSTANTS_H
