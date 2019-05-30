#include "tga_replace_pal.hpp"

namespace tractor_converter{



void tga_replace_pal_mode(
  const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      "source_dir",
      "output_dir",
      "pal_dir",
    };
    helpers::check_options(options,options_to_check);


    boost::filesystem::path source_dir =
      helpers::get_directory(options["source_dir"].as<std::string>(),
                             "source_dir");
    boost::filesystem::path output_dir =
      helpers::get_directory(options["output_dir"].as<std::string>(),
                             "output_dir");
    boost::filesystem::path pal_dir =
      helpers::get_directory(options["pal_dir"].as<std::string>(),
                             "pal_dir");

    for(const auto &file : boost::filesystem::directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(file.status()) &&
         file.path().extension().string() == ".tga")
      {
        // Leaving extra 768 bytes at the beginning of
        // *.tga file string to move header here in case
        // original palette is smaller than default 768.
        std::size_t original_start_of_image = tga_default_pal_size;
        std::string bytes =
          helpers::read_file(
            file.path(),
            helpers::file_flag::binary | helpers::file_flag::read_all,
            original_start_of_image,
            0,
            helpers::read_all_dummy_size,
            "source_dir");

        helpers::tga tga_image(bytes,
                               original_start_of_image,
                               file.path().string());

        boost::filesystem::path new_pal_file = pal_dir;
        new_pal_file.append(file.path().stem().string() + ".pal",
                            boost::filesystem::path::codecvt());
        std::string new_pal =
          helpers::read_file(
            new_pal_file,
            helpers::file_flag::binary | helpers::file_flag::read_all,
            0,
            0,
            helpers::read_all_dummy_size,
            "pal_dir");



        std::size_t missing_byte_num =
          tga_default_pal_size - tga_image.pal_size;
        std::size_t new_start_of_image =
          original_start_of_image - missing_byte_num;
        std::size_t tga_header_n_ID_field_size =
          tga_header_size + tga_image.ID_field_length;
        // Moving new header and pal to new position
        // in case palette size changed.
        if(missing_byte_num)
        {
          // changing color size to 256
          bytes.replace(
            original_start_of_image + tga_color_map_length_pos,
            tga_color_map_length.size(),
            tga_color_map_length);

          // moving header to new position
          bytes.replace(
            new_start_of_image, tga_header_n_ID_field_size,
            bytes,
            original_start_of_image, tga_header_n_ID_field_size);
        }
        // moving new pal to new position
        bytes.replace(
          new_start_of_image + tga_header_n_ID_field_size,
          tga_default_pal_size,
          new_pal);



        boost::filesystem::path file_to_save = output_dir;
        file_to_save.append(file.path().stem().string() + ".tga",
                            boost::filesystem::path::codecvt());
        std::size_t size_of_file_to_write =
          tga_header_size +
          tga_image.ID_field_length +
          tga_default_pal_size +
          tga_image.raw_bitmap_size;
        helpers::write_to_file(
          file_to_save,
          bytes,
          helpers::file_flag::overwrite | helpers::file_flag::binary,
          new_start_of_image,
          0,
          size_of_file_to_write,
          "output_dir");
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << "tga_replace_pal_mode mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
