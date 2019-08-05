#include "tga_merge_unused_pal.hpp"

namespace tractor_converter{



void tga_merge_unused_pal_mode(
  const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      option::name::source_dir,
      option::name::output_dir,
      option::name::unused_pals_dir,
    };
    helpers::check_options(options,options_to_check);


    boost::filesystem::path source_dir =
      helpers::get_directory(
        options[option::name::source_dir].as<std::string>(),
        option::name::source_dir);
    boost::filesystem::path output_dir =
      helpers::get_directory(
        options[option::name::output_dir].as<std::string>(),
        option::name::output_dir);
    boost::filesystem::path unused_pals_dir =
      helpers::get_directory(
        options[option::name::unused_pals_dir].as<std::string>(),
        option::name::unused_pals_dir);

    for(const auto &file : boost::filesystem::directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(file.status()) &&
         file.path().extension().string() == ext::tga)
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
            option::name::source_dir);

        helpers::tga tga_image(bytes,
                               original_start_of_image,
                               file.path().string());

        boost::filesystem::path unused_pal_file = unused_pals_dir;
        unused_pal_file.append(file.path().stem().string() + ext::pal,
                               boost::filesystem::path::codecvt());
        std::string unused_pal =
          helpers::read_file(
            unused_pal_file,
            helpers::file_flag::binary | helpers::file_flag::read_all,
            0,
            0,
            helpers::read_all_dummy_size,
            option::name::unused_pals_dir);



        // counting number of null bytes colors from the end in tga palette
        std::size_t null_colors = 0;
        std::vector<bool> orig_pal_null_map(tga_default_colors_num_in_pal,
                                            false);
        for(std::size_t current_color_num = 0,
              current_color_pos = tga_image.pal_start_pos,
              end_color = tga_image.pal_start_pos +
                          tga_image.pal_size;
            current_color_pos != end_color;
            ++current_color_num,
              current_color_pos += 3)
        {
          if(!helpers::check_pal_color_used(current_color_pos, bytes))
          {
            ++null_colors;
            orig_pal_null_map[current_color_num] = true;
          }
        }
        std::size_t used_pal_colors = tga_image.colors_num - null_colors;

        // counting number of colors in unused_pal
        std::size_t unused_pal_colors_num = 0;
        std::vector<bool> unused_pal_usage_map(tga_default_colors_num_in_pal,
                                               false);
        for(std::size_t current_color_num = 0,
              current_color_pos = 0,
              end_color = tga_default_pal_size;
            current_color_pos != end_color;
            ++current_color_num,
              current_color_pos += 3)
        {
          if(helpers::check_pal_color_used(current_color_pos, unused_pal))
          {
            ++unused_pal_colors_num;
            unused_pal_usage_map[current_color_num] = true;
          }
        }

        if(used_pal_colors + unused_pal_colors_num >
           tga_default_colors_num_in_pal)
        {
          throw std::runtime_error(
            "Number of palette colors of image " + file.path().string() +
            " is " + std::to_string(used_pal_colors) +
            ", number of colors in palette " + unused_pal_file.string() +
            " is " + std::to_string(unused_pal_colors_num) +
            ". Their sum is greater than expected max of " +
            std::to_string(tga_default_colors_num_in_pal) + ".");
        }



        // Creating merged palette.
        // All color shifts of original palette
        // are recorded to new_palette_pos_change_map
        // to change values of all bytes in image.
        std::string new_palette(tga_default_pal_size, '\0');
        std::vector<unsigned char> new_palette_pos_change_map(
          tga_default_colors_num_in_pal,
          0);
        for(std::size_t tga_pal_color_num = 0,
              tga_pal_color_pos = tga_image.pal_start_pos,
              usage_pal_color_num = 0,
              new_pal_pos = 0;
            new_pal_pos != tga_default_pal_size;
            ++usage_pal_color_num,
              new_pal_pos += 3)
        {
          if(unused_pal_usage_map[usage_pal_color_num])
          {
            new_palette.replace(
              new_pal_pos, tga_default_color_size,
              unused_pal,
              new_pal_pos, tga_default_color_size);
            // No need to shift if it's null bytes color.
            if(orig_pal_null_map[usage_pal_color_num])
            {
              ++tga_pal_color_num;
              tga_pal_color_pos += 3;
            }
            else
            {
              for(std::size_t cur_color_for_map = tga_pal_color_num,
                    end_color_for_map = tga_default_colors_num_in_pal;
                  cur_color_for_map != end_color_for_map;
                  ++cur_color_for_map)
              {
                ++new_palette_pos_change_map[cur_color_for_map];
              }
            }
          }
          else
          {
            if(tga_pal_color_num < tga_image.colors_num)
            {
              new_palette.replace(
                new_pal_pos, tga_default_color_size,
                bytes,
                tga_pal_color_pos, tga_default_color_size);
              ++tga_pal_color_num;
              tga_pal_color_pos += 3;
            }
            else
            {
              new_palette.replace(new_pal_pos,
                                  tga_default_color_size,
                                  tga_default_color_size,
                                  '\0');
            }
          }
        }



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
          new_palette);

        // Changing image bytes as needed
        // TEST
        /*
        for(std::size_t cur_num = 0, max_num = 256;
            cur_num != max_num;
            ++cur_num)
        {
          std::cout << cur_num << " : " <<
            static_cast<std::size_t>(new_palette_pos_change_map[cur_num]) <<
            '\n';
        }
        */
        for(std::size_t current_byte = tga_image.raw_bitmap_start_pos,
              end_byte = tga_image.raw_bitmap_start_pos +
                         tga_image.raw_bitmap_size;
            current_byte != end_byte;
            ++current_byte)
        {
          bytes[current_byte] =
            static_cast<char>(
              static_cast<unsigned char>(bytes[current_byte]) +
              new_palette_pos_change_map[
                static_cast<unsigned char>(bytes[current_byte])]);
        }



        boost::filesystem::path file_to_save = output_dir;
        file_to_save.append(file.path().stem().string() + ext::tga,
                            boost::filesystem::path::codecvt());
//      helpers::save_file(file_to_save,
//                         bytes,
//                         helpers::file_flag::binary,
//                         option::name::output_dir);
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
          option::name::output_dir);
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << mode::name::tga_merge_unused_pal << " mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
