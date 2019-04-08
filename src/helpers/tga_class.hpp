#ifndef TRACTOR_CONVERTER_TGA_CLASS_H
#define TRACTOR_CONVERTER_TGA_CLASS_H

#include "defines.hpp"
#include "tga_constants.hpp"

#include "raw_num_operations.hpp"

#include <exception>
#include <stdexcept>

#include <cstddef>
#include <string>



namespace tractor_converter{
namespace helpers{

std::size_t get_color_size(
  const std::string &bytes,
  const std::size_t image_start_pos,
  const std::string &path_string,
  const int throw_on_failure = TRACTOR_CONVERTER_THROW_ON_FAILURE);

std::size_t get_colors_num(
  const std::string &bytes,
  const std::size_t image_start_pos,
  const std::string &path_string,
  const int throw_on_failure = TRACTOR_CONVERTER_THROW_ON_FAILURE);

struct tga
{
  public:

  tga(const std::string &bytes_arg,
      const std::size_t image_start_pos,
      const std::string &file_name_error);

  std::string *bytes;

  std::size_t ID_field_length;
  int color_map_type;
  int image_type;

  std::size_t color_size;
  std::size_t colors_num;

  std::size_t pal_size;

  std::size_t pal_start_pos;

  std::string width_height;

  std::uint16_t width;
  std::uint16_t height;
  std::size_t raw_bitmap_size;

  std::size_t raw_bitmap_start_pos;

};

} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_TGA_CLASS_H
