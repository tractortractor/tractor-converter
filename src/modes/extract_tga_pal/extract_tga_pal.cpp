#include "extract_tga_pal.hpp"

namespace tractor_converter{



void extract_tga_pal_mode(const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      "source_dir",
      "output_dir",
    };
    helpers::check_options(options,options_to_check);



    boost::filesystem::path source_dir =
      helpers::get_directory(
        options["source_dir"].as<std::string>(),
        "source_dir");
    boost::filesystem::path output_dir =
      helpers::get_directory(
        options["output_dir"].as<std::string>(),
        "output_dir");

    for(const auto &file : boost::filesystem::directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(file.status()) &&
         file.path().extension().string() == ".tga")
      {
        std::string bytes =
          helpers::read_file(
            file.path(),
            helpers::file_flag::binary | helpers::file_flag::read_all,
            0,
            0,
            helpers::read_all_dummy_size,
            "source_dir");



        helpers::tga tga_image(bytes, 0, file.path().string());



        if(tga_image.pal_size < tga_default_pal_size)
        {
          std::size_t missing_color_bytes =
            (tga_default_pal_size - tga_image.pal_size);
          bytes.replace(tga_image.pal_start_pos + tga_image.pal_size,
                        missing_color_bytes,
                        missing_color_bytes,
                        '\0');
        }


        boost::filesystem::path file_to_save = output_dir;
        file_to_save.append(file.path().stem().string() + ".pal",
                            boost::filesystem::path::codecvt());
        helpers::write_to_file(
          file_to_save,
          bytes,
          helpers::file_flag::overwrite | helpers::file_flag::binary,
          tga_image.pal_start_pos,
          0,
          tga_default_pal_size,
          "output_dir");
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << "extract_tga_pal mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
