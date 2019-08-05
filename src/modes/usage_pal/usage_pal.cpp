#include "usage_pal.hpp"

namespace tractor_converter{



void usage_pal_mode_save_output_readable(
  boost::filesystem::path file,
  const std::vector<int> &used_characters,
  const std::string &file_name_error)
{
  std::string used_bytes_map_readable;
  // 256 bytes total *
  //   (2 size of hex of byte + 1 space + 10 max int + 2 newline) +
  // 13 "Used colors: " string + 10 max int
  used_bytes_map_readable.reserve(
    tga_default_colors_num_in_pal *
      (2 + 1 + 10 + 2) +
    13 + 10);

  std::size_t used_colors = 0;
  for(std::size_t current_byte = 0;
      current_byte != tga_default_colors_num_in_pal;
      ++current_byte)
  {
    used_bytes_map_readable.append(
      helpers::int_to_hex_string<std::size_t>(current_byte));
    used_bytes_map_readable.push_back(' ');
    used_bytes_map_readable.append(
      std::to_string(used_characters[current_byte]));
    used_bytes_map_readable.push_back('\r');
    used_bytes_map_readable.push_back('\n');
    if(used_characters[current_byte])
    {
      ++used_colors;
    }
  }
  used_bytes_map_readable.append(
    "Used colors: " + std::to_string(used_colors));
  helpers::save_file(file,
                     used_bytes_map_readable,
                     helpers::file_flag::binary,
                     file_name_error);
}

void usage_pal_mode_save_output_not_readable(
  boost::filesystem::path file,
  const std::vector<int> &used_characters,
  const std::string &file_name_error)
{
  std::string used_bytes_map;
  // 256 colors total * 3 color bytes
  used_bytes_map.reserve(
    tga_default_colors_num_in_pal * tga_default_color_size);

  for(std::size_t current_byte = 0;
      current_byte != tga_default_colors_num_in_pal;
      ++current_byte)
  {
    if(used_characters[current_byte])
    {
      // tga_default_color_size = 3
      used_bytes_map.append(tga_default_color_size, '\xFF');
    }
    else
    {
      // tga_default_color_size = 3
      used_bytes_map.append(tga_default_color_size, '\0');
    }
  }
  helpers::save_file(file,
                     used_bytes_map,
                     helpers::file_flag::binary,
                     file_name_error);
}

void usage_pal_mode_save_output(
  boost::filesystem::path file,
  const std::vector<int> &used_characters,
  bool readable_output,
  const std::string &file_name_error)
{
  // Getting human readable map.
  if(readable_output)
  {
    usage_pal_mode_save_output_readable(file,
                                        used_characters,
                                        file_name_error);
  }
  else
  {
    usage_pal_mode_save_output_not_readable(file,
                                            used_characters,
                                            file_name_error);
  }
}



// Trying to determine which byte values are used in Vangers BMP files
void usage_pal_mode(const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      option::name::source_dir,
    };
    helpers::check_options(options, options_to_check);

    std::vector<std::string> usage_pal_options_to_check;
    usage_pal_options_to_check.reserve(1);
    if(options[option::name::usage_pal_for_each_file].as<bool>())
    {
      usage_pal_options_to_check.push_back(option::name::output_dir);
    }
    else
    {
      usage_pal_options_to_check.push_back(option::name::output_file);
    }
    helpers::check_options(options, usage_pal_options_to_check);



    boost::filesystem::path source_dir =
      helpers::get_directory(
        options[option::name::source_dir].as<std::string>(),
        option::name::source_dir);

    boost::filesystem::path output_dir;
    if(options[option::name::usage_pal_for_each_file].as<bool>())
    {
      output_dir =
        helpers::get_directory(
          options[option::name::output_dir].as<std::string>(),
          option::name::output_dir);
    }

    std::vector<int> used_characters(tga_default_colors_num_in_pal, 0);

    for(const auto &file : boost::filesystem::directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(file.status()) &&
         file.path().extension().string() == ".bmp")
      {
        std::string bmp_map;
        // First 4 bytes indicate width and height of BMP file
        // so they are skipped.
        bmp_map =
          helpers::read_file(
            file.path(),
            helpers::file_flag::binary | helpers::file_flag::read_all,
            0,
            4,
            helpers::read_all_dummy_size,
            option::name::source_dir);

        for(char& bmp_byte : bmp_map)
        {
          ++used_characters[
            static_cast<std::size_t>(static_cast<unsigned char>(bmp_byte))];
        }

        if(options[option::name::usage_pal_for_each_file].as<bool>())
        {
          boost::filesystem::path file_to_save = output_dir;
          if(options[option::name::readable_output].as<bool>())
          {
            file_to_save.append(file.path().stem().string() + ".txt",
                                boost::filesystem::path::codecvt());
          }
          else
          {
            file_to_save.append(file.path().stem().string() + ".pal",
                                boost::filesystem::path::codecvt());
          }
          usage_pal_mode_save_output(
            file_to_save,
            used_characters,
            options[option::name::readable_output].as<bool>(),
            option::name::output_dir);
          // delete all values
          std::fill(used_characters.begin(), used_characters.end(), 0);
        }
      }
    }

    if(!options[option::name::usage_pal_for_each_file].as<bool>())
    {
      boost::filesystem::path file_to_save =
        boost::filesystem::system_complete(
          options[option::name::output_file].as<std::string>());
      usage_pal_mode_save_output(
        file_to_save,
        used_characters,
        options[option::name::readable_output].as<bool>(),
        option::name::output_file);
    }
  }
  catch(std::exception &)
  {
    std::cout << mode::name::usage_pal << " mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
