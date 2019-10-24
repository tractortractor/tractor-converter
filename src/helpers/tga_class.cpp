#include "tga_class.hpp"



namespace tractor_converter{
namespace helpers{



std::size_t get_color_size(
  const std::string &bytes,
  const std::size_t image_start_pos,
  const std::string &path_string,
  const bitflag<error_handling> error_flags)
{
  const std::size_t color_size =
    static_cast<std::size_t>(static_cast<unsigned char>(
      bytes[image_start_pos + tga_color_map_entry_size_pos])) /
    CHAR_BIT;
  if(color_size != tga_default_color_size)
  {
    if(error_flags & error_handling::throw_exception)
    {
      throw std::runtime_error(
        "Image " + path_string + " has color depth size " +
        std::to_string(color_size) +
        " bytes which is different from expected " +
        std::to_string(tga_default_color_size) + " bytes.");
    }
    else
    {
      return 0;
    }
  }
  return color_size;
}



std::size_t get_colors_num(
  const std::string &bytes,
  const std::size_t image_start_pos,
  const std::string &path_string,
  const bitflag<error_handling> error_flags)
{
  std::uint16_t colors_num =
    raw_bytes_to_num<std::uint16_t>(
      bytes,
      image_start_pos + tga_color_map_length_pos);

  if(colors_num > tga_default_colors_num_in_pal)
  {
    if(error_flags & error_handling::throw_exception)
    {
      throw std::runtime_error(
        "Image " + path_string + " has number of colors in the palette " +
        std::to_string(colors_num) + " which is greater than expected max " +
        std::to_string(tga_default_colors_num_in_pal) + " colors.");
    }
    else
    {
      return 0;
    }
  }
  return colors_num;
}



tga::tga(const std::string &bytes_arg,
         const std::size_t image_start_pos,
         const std::string &file_name_error)
{
  const std::string *bytes = &bytes_arg;

  ID_field_length =
    static_cast<std::size_t>(
      static_cast<unsigned char>(
        (*bytes)[image_start_pos + tga_id_length_pos]));

  color_map_type =
    static_cast<int>(
      static_cast<unsigned char>(
        (*bytes)[image_start_pos + tga_color_map_type_pos]));
  if(color_map_type != tga_color_map_type_expected)
  {
    std::string err_msg =
      "Image " + file_name_error +
      " has color map type " + std::to_string(color_map_type) + ".\n";
    if(color_map_type == tga_color_map_type_empty)
    {
      err_msg.append("Color map is not present.\n");
    }
    else
    {
      err_msg.append("Unexpected color map type.\n");
    }
    throw std::runtime_error(err_msg);
  }

  image_type =
    static_cast<int>(
      static_cast<unsigned char>(
        (*bytes)[image_start_pos + tga_image_type_pos]));
  if(image_type != tga_image_type_expected)
  {
    std::string err_msg =
      "Image " + file_name_error +
      " has image type " + std::to_string(image_type);
    if(image_types.count(image_type))
    {
      err_msg.append(" which means \"" + image_types.at(image_type) + "\"");
    }
    err_msg.append(
      ".\n"
      "Expected image type " + std::to_string(tga_image_type_expected) +
      " which means \"" + image_types.at(tga_image_type_expected) + "\".\n");
    throw std::runtime_error(err_msg);
  }

  color_size = get_color_size(*bytes, image_start_pos, file_name_error);
  colors_num = get_colors_num(*bytes, image_start_pos, file_name_error);

  pal_size = colors_num * color_size;


  pal_start_pos = image_start_pos + tga_header_size + ID_field_length;

  width_height =
    bytes->substr(image_start_pos + tga_coords_pos, tga_coords_size);

  width =  raw_bytes_to_num<std::uint16_t>(width_height, 0);
  height = raw_bytes_to_num<std::uint16_t>(width_height, 2);

  raw_bitmap_size = width * height;

  raw_bitmap_start_pos = pal_start_pos + pal_size;
}



} // namespace helpers
} // namespace tractor_converter
