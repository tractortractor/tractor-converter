#include "pal_shift_for_vangers_avi.hpp"

namespace tractor_converter{



void pal_shift_for_vangers_avi_mode(
  const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      "source_dir",
      "output_dir",
    };
    helpers::check_options(options, options_to_check);



    boost::filesystem::path source_dir =
      helpers::get_directory(options["source_dir"].as<std::string>(),
                             "source_dir");
    boost::filesystem::path output_dir =
      helpers::get_directory(options["output_dir"].as<std::string>(),
                             "output_dir");

    for(const auto &file : boost::filesystem::directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(file.status()) &&
         file.path().extension().string() == ".pal")
      {
        // Reading first half of the palette from file
        // and writing it to second half of the palette of source_pal string.
        std::string output_pal = helpers::read_file(file.path(),
                                                    TRACTOR_CONVERTER_BINARY,
                                                    tga_default_pal_size/2,
                                                    0,
                                                    tga_default_pal_size/2,
                                                    "source_dir");

        boost::filesystem::path file_to_save = output_dir;
        file_to_save.append(file.path().stem().string() + ".pal",
                            boost::filesystem::path::codecvt());
        helpers::save_file(file_to_save,
                           output_pal,
                           TRACTOR_CONVERTER_BINARY,
                           "output_dir");
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << "pal_shift_for_vangers_avi mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
