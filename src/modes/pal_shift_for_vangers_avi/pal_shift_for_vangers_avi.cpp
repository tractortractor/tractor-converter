#include "pal_shift_for_vangers_avi.hpp"



namespace tractor_converter{



void pal_shift_for_vangers_avi_mode(
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
        // Reading first half of the palette from file
        // and writing it to second half of the palette of source_pal string.
        std::string output_pal =
          helpers::read_file(file.path(),
                             helpers::file_flag::binary,
                             tga_default_pal_size / 2,
                             0,
                             tga_default_pal_size / 2,
                             option::name::source_dir);

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
    std::cout << mode::name::pal_shift_for_vangers_avi <<
      " mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
