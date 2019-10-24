#include "tga_to_bmp.hpp"



namespace tractor_converter{



void tga_to_bmp_mode(const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      option::name::source_dir,
      option::name::output_dir,
    };
    helpers::check_options(options, options_to_check);
    if(options[option::name::items_bmp].as<bool>())
    {
      const std::vector<std::string> items_options_to_check =
      {
        option::name::output_dir_through_map,
        option::name::map,
      };
      helpers::check_options(options, items_options_to_check);
    }



    boost::filesystem::path source_dir =
      helpers::get_directory(
        options[option::name::source_dir].as<std::string>(),
        option::name::source_dir);
    boost::filesystem::path output_dir =
      helpers::get_directory(
        options[option::name::output_dir].as<std::string>(),
        option::name::output_dir);

    std::string compare_map;
    boost::filesystem::path output_dir_through_map;
    if(options[option::name::items_bmp].as<bool>())
    {
      compare_map =
        helpers::read_file(
          options[option::name::map].as<std::string>(),
          helpers::file_flag::binary | helpers::file_flag::read_all,
          0,
          0,
          helpers::read_all_dummy_size,
          option::name::map);
      output_dir_through_map =
        helpers::get_directory(
          options[option::name::output_dir_through_map].as<std::string>(),
          option::name::output_dir_through_map);
    }

    for(const auto &file : boost::filesystem::directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(file.status()) &&
         boost::algorithm::to_lower_copy(file.path().extension().string()) ==
           ext::tga)
      {
        std::string bytes =
          helpers::read_file(
            file.path(),
            helpers::file_flag::binary | helpers::file_flag::read_all,
            0,
            0,
            helpers::read_all_dummy_size,
            option::name::source_dir);

        helpers::tga tga_image(bytes, 0, file.path().string());

        std::size_t vangers_bmp_size =
          vangers_bmp_coords_size + tga_image.raw_bitmap_size;

        const std::size_t real_start_of_bmp_no_coords =
          tga_image.raw_bitmap_start_pos;

        // Using vangers_bmp_coords_size bytes before start for coordinates.
        // Last 4 bytes of palette are overwritten,
        // so we need to do all operations with palette
        // before inserting coordinates and saving new *.bmp.
        const std::size_t real_start_of_bmp =
          real_start_of_bmp_no_coords - vangers_bmp_coords_size;


        if(options[option::name::fix_null_bytes_and_direction].as<bool>())
        {
          // Changing all bytes with value
          // which is not used by palette into null bytes.
          std::vector<int> used_characters(tga_default_colors_num_in_pal, 0);
          for(std::size_t current_color_num = 0,
                current_color_pos = tga_image.pal_start_pos,
                end_color = real_start_of_bmp_no_coords;
              current_color_pos != end_color;
              ++current_color_num,
                current_color_pos += 3)
          {
            if(helpers::check_pal_color_used(current_color_pos, bytes))
            {
              ++used_characters[current_color_num];
            }
          }
          for(std::size_t current_byte = real_start_of_bmp_no_coords,
                end_of_bitmap = real_start_of_bmp + vangers_bmp_size;
              current_byte != end_of_bitmap;
              ++current_byte)
          {
            std::size_t current_color_to_check =
              static_cast<std::size_t>(
                static_cast<unsigned char>(bytes[current_byte]));
            if(!used_characters[current_color_to_check])
            {
              bytes[current_byte] = '\0';
            }
          }


          // Turning if needed.
          std::string tga_image_descriptor =
            bytes.substr(tga_image_specification_image_descriptor_pos,
                         tga_image_specification_image_descriptor_str.size());
          // Checking whether image descriptor is expected by Vangers.
          if(tga_image_descriptor !=
             tga_image_specification_image_descriptor_str)
          {
            // Checking whether bit 4 is off counting from 0.
            // If so, flip horizontally.
            if(tga_image_descriptor[0] & '\x10')
            {
              std::size_t row_start_pos = real_start_of_bmp_no_coords;
              for(std::size_t current_y = 0;
                  current_y != tga_image.height;
                  ++current_y)
              {
                for(std::size_t current_x = 0, max_x = tga_image.width / 2;
                    current_x != max_x;
                    ++current_x)
                {
                  std::size_t byte_to_flip_1_pos = row_start_pos + current_x;
                  std::size_t byte_to_flip_2_pos =
                    row_start_pos + tga_image.width - 1 - current_x;
                  char temp = bytes[byte_to_flip_1_pos];
                  bytes[byte_to_flip_1_pos] = bytes[byte_to_flip_2_pos];
                  bytes[byte_to_flip_2_pos] = temp;
                }
                row_start_pos += tga_image.width;
              }
            }
            // Checking whether bit 5 is off counting from 0.
            // If so, flip vertically.
            if(!(tga_image_descriptor[0] & '\x20'))
            {
              std::size_t column_start_pos = real_start_of_bmp_no_coords;
              for(std::size_t current_x = 0;
                  current_x != tga_image.width;
                  ++current_x)
              {
                for(std::size_t current_y = 0, max_y = tga_image.height / 2;
                    current_y != max_y;
                    ++current_y)
                {
                  std::size_t byte_to_flip_1_pos =
                    column_start_pos + current_y * tga_image.width;
                  std::size_t byte_to_flip_2_pos =
                    column_start_pos +
                    (tga_image.height - 1 - current_y) * tga_image.width;
                  char temp = bytes[byte_to_flip_1_pos];
                  bytes[byte_to_flip_1_pos] = bytes[byte_to_flip_2_pos];
                  bytes[byte_to_flip_2_pos] = temp;
                }
                ++column_start_pos;
              }
            }
          }
        }

        bytes.replace(real_start_of_bmp,
                      vangers_bmp_coords_size,
                      tga_image.width_height);



        boost::filesystem::path file_to_save = output_dir;
        file_to_save.append(
          boost::algorithm::to_lower_copy(file.path().stem().string()) +
            ext::bmp,
          boost::filesystem::path::codecvt());
        helpers::write_to_file(
          file_to_save,
          bytes,
          helpers::file_flag::overwrite | helpers::file_flag::binary,
          real_start_of_bmp,
          0,
          vangers_bmp_size,
          option::name::output_dir);



        if(options[option::name::items_bmp].as<bool>())
        {
          std::string mapped_bytes;
          mapped_bytes.resize(vangers_bmp_size, '\0');
          mapped_bytes.replace(vangers_bmp_coords_pos,
                               vangers_bmp_coords_size,
                               tga_image.width_height);

          for(std::size_t current_byte = real_start_of_bmp_no_coords,
                current_mapped_byte = vangers_bmp_coords_size,
                mapped_bytes_size = vangers_bmp_size;
              current_mapped_byte != mapped_bytes_size;
              ++current_byte,
                ++current_mapped_byte)
          {
            mapped_bytes[current_mapped_byte] =
              compare_map[static_cast<std::size_t>(
                static_cast<unsigned char>(bytes[current_byte]))];
          }

          boost::filesystem::path file_to_save_mapped = output_dir_through_map;
          file_to_save_mapped.append(
            boost::algorithm::to_lower_copy(file.path().stem().string()) +
              ext::bmp,
            boost::filesystem::path::codecvt());
          helpers::save_file(file_to_save_mapped,
                             mapped_bytes,
                             helpers::file_flag::binary,
                             option::name::output_dir_through_map);
        }
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << mode::name::tga_to_bmp << " mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
