#include "vangers_pal_to_tga_pal.hpp"



namespace tractor_converter{



void vangers_pal_to_tga_pal_mode(
  const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      option::name::source_dir,
      option::name::output_dir,
    };
    helpers::check_options(options, options_to_check);



    boost::filesystem::path source_dir =
      helpers::get_directory(
        options[option::name::source_dir].as<std::string>(),
        option::name::source_dir);
    boost::filesystem::path output_dir =
      helpers::get_directory(
        options[option::name::output_dir].as<std::string>(),
        option::name::output_dir);

    for(const auto &file : boost::filesystem::directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(file.status()) &&
         boost::algorithm::to_lower_copy(file.path().extension().string()) ==
           ext::pal)
      {
        std::string source_pal =
          helpers::read_file(file.path(),
                             helpers::file_flag::binary,
                             0,
                             0,
                             tga_default_pal_size,
                             option::name::source_dir);
        std::string output_pal(tga_default_pal_size, '\0');

        for(std::size_t current_pal_byte = 0;
            current_pal_byte != tga_default_pal_size;
            current_pal_byte += 3)
        {
          //// VANGERS SOURCE
          //XGR_Palette->colors[i].r = pal[i].R << 2;
          //XGR_Palette->colors[i].g = pal[i].G << 2;
          //XGR_Palette->colors[i].b = pal[i].B << 2;

          if(!options[option::name::reversed].as<bool>())
          {
            output_pal[current_pal_byte + 0] =
              source_pal[current_pal_byte + 2] << 2;
            output_pal[current_pal_byte + 1] =
              source_pal[current_pal_byte + 1] << 2;
            output_pal[current_pal_byte + 2] =
              source_pal[current_pal_byte + 0] << 2;
          }
          else
          {
            output_pal[current_pal_byte + 0] =
              source_pal[current_pal_byte + 2] >> 2;
            output_pal[current_pal_byte + 1] =
              source_pal[current_pal_byte + 1] >> 2;
            output_pal[current_pal_byte + 2] =
              source_pal[current_pal_byte + 0] >> 2;
          }
        }

        boost::filesystem::path file_to_save = output_dir;
        file_to_save.append(
          boost::algorithm::to_lower_copy(file.path().stem().string()) +
            ext::pal,
          boost::filesystem::path::codecvt());
        helpers::save_file(file_to_save,
                           output_pal,
                           helpers::file_flag::binary,
                           option::name::output_dir);
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << mode::name::vangers_pal_to_tga_pal << " mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
