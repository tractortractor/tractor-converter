#include "remove_not_used_pal.hpp"

namespace tractor_converter{



// Trying to determine
// which byte values are used in Vangers *.bmp files
void remove_not_used_pal_mode(
  const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      "source_dir",
      "output_dir",
      "output_dir_unused",
      "usage_pals_dir",
    };
    helpers::check_options(options,options_to_check);



    boost::filesystem::path source_dir =
      helpers::get_directory(options["source_dir"].as<std::string>(),
                             "source_dir");
    boost::filesystem::path output_dir =
      helpers::get_directory(options["output_dir"].as<std::string>(),
                             "output_dir");
    boost::filesystem::path output_dir_unused =
      helpers::get_directory(options["output_dir_unused"].as<std::string>(),
                             "output_dir_unused");
    boost::filesystem::path usage_pals_dir =
      helpers::get_directory(options["usage_pals_dir"].as<std::string>(),
                             "usage_pals_dir");

    for(const auto &file : boost::filesystem::directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(file.status()) &&
         file.path().extension().string() == ".pal")
      {
        std::string orig_pal =
          helpers::read_file(file.path(),
                             TRACTOR_CONVERTER_BINARY,
                             0,
                             0,
                             TRACTOR_CONVERTER_FILE_READ_ALL,
                             "source_dir");

        boost::filesystem::path usage_pal_file = usage_pals_dir;
        usage_pal_file.append(file.path().stem().string() + ".pal",
                              boost::filesystem::path::codecvt());
        std::string usage_pal =
          helpers::read_file(usage_pal_file,
                             TRACTOR_CONVERTER_BINARY,
                             0,
                             0,
                             TRACTOR_CONVERTER_FILE_READ_ALL,
                             "usage_pals_dir");


        std::string pal_used(tga_default_pal_size, '\0');
        std::string pal_unused(tga_default_pal_size, '\0');

        for(std::size_t current_color_pos = 0;
            current_color_pos != tga_default_pal_size;
            current_color_pos += tga_default_color_size)
        {
          if(helpers::check_pal_color_used(current_color_pos, usage_pal))
          {
            pal_used.replace(
              current_color_pos, tga_default_color_size,
              orig_pal,
              current_color_pos, tga_default_color_size);
          }
          else
          {
            pal_unused.replace(
              current_color_pos, tga_default_color_size,
              orig_pal,
              current_color_pos, tga_default_color_size);
          }
        }

        boost::filesystem::path file_to_save = output_dir;
        file_to_save.append(file.path().stem().string() + ".pal",
                            boost::filesystem::path::codecvt());
        helpers::save_file(file_to_save,
                           pal_used,
                           TRACTOR_CONVERTER_BINARY,
                           "output_dir");

        boost::filesystem::path file_to_save_unused = output_dir_unused;
        file_to_save_unused.append(file.path().stem().string() + ".pal",
                                   boost::filesystem::path::codecvt());
        helpers::save_file(file_to_save_unused,
                           pal_unused,
                           TRACTOR_CONVERTER_BINARY,
                           "output_dir_unused");
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << "remove_not_used_pal mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
