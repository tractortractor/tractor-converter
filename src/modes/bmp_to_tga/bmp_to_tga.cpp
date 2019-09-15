#include "bmp_to_tga.hpp"



namespace tractor_converter{



void bmp_to_tga_mode(const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      option::name::source_dir,
      option::name::output_dir,
    };
    helpers::check_options(options, options_to_check);

    std::vector<std::string> pal_options_to_check;
    pal_options_to_check.reserve(1);
    if(options[option::name::pal_for_each_file].as<bool>())
    {
      pal_options_to_check.push_back(option::name::pal_dir);
    }
    else
    {
      pal_options_to_check.push_back(option::name::pal);
    }
    helpers::check_options(options, pal_options_to_check);


    std::string palette;
    boost::filesystem::path palette_dir;

    boost::filesystem::path source_dir = 
      helpers::get_directory(
        options[option::name::source_dir].as<std::string>(),
        option::name::source_dir);
    if(options[option::name::pal_for_each_file].as<bool>())
    {
      palette_dir =
        helpers::get_directory(
          options[option::name::pal_dir].as<std::string>(),
          option::name::pal_dir);
    }
    else
    {
      palette =
        helpers::read_file(
          options[option::name::pal].as<std::string>(),
          helpers::file_flag::binary | helpers::file_flag::read_all,
          0,
          0,
          helpers::read_all_dummy_size,
          option::name::pal);
    }
    boost::filesystem::path output_dir =
      helpers::get_directory(
        options[option::name::output_dir].as<std::string>(),
        option::name::output_dir);

    for(const auto &file : boost::filesystem::directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(file.status()) &&
         boost::algorithm::to_lower_copy(file.path().extension().string()) ==
           ext::bmp)
      {
        std::string bytes =
          helpers::read_file(
            file.path(),
            helpers::file_flag::binary | helpers::file_flag::read_all,
            tga_default_header_and_pal_size - vangers_bmp_coords_size,
            0,
            helpers::read_all_dummy_size,
            option::name::source_dir);

        std::string current_coords =
          bytes.substr(
            tga_default_header_and_pal_size - vangers_bmp_coords_size,
            vangers_bmp_coords_size);


        if(options[option::name::pal_for_each_file].as<bool>())
        {
          boost::filesystem::path palette_file =
            helpers::filepath_case_insensitive_part_get(
              palette_dir,
              file.path().stem().string() + ext::pal);
          palette =
            helpers::read_file(
              palette_file,
              helpers::file_flag::binary | helpers::file_flag::read_all,
              0,
              0,
              helpers::read_all_dummy_size,
              option::name::pal_dir);
        }


        // Inserting header.
        bytes.replace(0, tga_header_size, tga_header_str);
        // Replacing dummy width and height with real ones.
        bytes.replace(tga_coords_pos, tga_coords_size, current_coords);
        // Inserting palette.
        bytes.replace(tga_default_pal_pos, tga_default_pal_size, palette);



        boost::filesystem::path file_to_save = output_dir;
        file_to_save.append(
          boost::algorithm::to_lower_copy(file.path().stem().string()) +
            ext::tga,
          boost::filesystem::path::codecvt());
        helpers::save_file(file_to_save,
                           bytes,
                           helpers::file_flag::binary,
                           option::name::output_dir);
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << mode::name::bmp_to_tga << " mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
