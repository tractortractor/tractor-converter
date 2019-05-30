#include "bmp_to_tga.hpp"

namespace tractor_converter{



void bmp_to_tga_mode(const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      "source_dir",
      "output_dir",
    };
    helpers::check_options(options, options_to_check);

    std::vector<std::string> pal_options_to_check;
    pal_options_to_check.reserve(1);
    if(options["pal_for_each_file"].as<bool>())
    {
      pal_options_to_check.push_back("pal_dir");
    }
    else
    {
      pal_options_to_check.push_back("pal");
    }
    helpers::check_options(options, pal_options_to_check);


    std::string palette;
    boost::filesystem::path palette_dir;

    boost::filesystem::path source_dir = 
      helpers::get_directory(options["source_dir"].as<std::string>(),
                             "source_dir");
    if(options["pal_for_each_file"].as<bool>())
    {
      palette_dir =
        helpers::get_directory(options["pal_dir"].as<std::string>(),
                               "pal_dir");
    }
    else
    {
      palette =
        helpers::read_file(
          options["pal"].as<std::string>(),
          helpers::file_flag::binary | helpers::file_flag::read_all,
          0,
          0,
          helpers::read_all_dummy_size,
          "pal");
    }
    boost::filesystem::path output_dir =
      helpers::get_directory(options["output_dir"].as<std::string>(),
                             "output_dir");

    for(const auto &file : boost::filesystem::directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(file.status()) &&
         file.path().extension().string() == ".bmp")
      {
        std::string bytes =
          helpers::read_file(
            file.path(),
            helpers::file_flag::binary | helpers::file_flag::read_all,
            tga_default_header_and_pal_size - vangers_bmp_coords_size,
            0,
            helpers::read_all_dummy_size,
            "source_dir");

        std::string current_coords =
          bytes.substr(
            tga_default_header_and_pal_size - vangers_bmp_coords_size,
            vangers_bmp_coords_size);


        if(options["pal_for_each_file"].as<bool>())
        {
          boost::filesystem::path palette_file = palette_dir;
          palette_file.append(
            file.path().stem().string() + ".pal",
            boost::filesystem::path::codecvt());
          palette =
            helpers::read_file(
              palette_file,
              helpers::file_flag::binary | helpers::file_flag::read_all,
              0,
              0,
              helpers::read_all_dummy_size,
              "pal_dir");
        }


        // inserting header
        bytes.replace(0, tga_header_size, tga_header_str);
        // replacing dummy width and height with real ones
        bytes.replace(tga_coords_pos, tga_coords_size, current_coords);
        // inserting palette
        bytes.replace(tga_default_pal_pos, tga_default_pal_size, palette);



        boost::filesystem::path file_to_save = output_dir;
        file_to_save.append(
          file.path().stem().string() + ".tga",
          boost::filesystem::path::codecvt());
        helpers::save_file(file_to_save,
                           bytes,
                           helpers::file_flag::binary,
                           "output_dir");
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << "bmp_to_tga mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
