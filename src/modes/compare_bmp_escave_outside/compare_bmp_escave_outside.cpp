#include "compare_bmp_escave_outside.hpp"



namespace tractor_converter{



// There are 2 BMP files with equal size.
// One for escave and other for outside of escave.
// In this mode program reads both files and checks it byte by byte.
// For example byte at position 0 in source_dir file is 0xFF
// while in same file of dir_to_compare it is 0xEE.
// So program records that for 0xFF byte there was 1 0xEE byte.
// At the end for each possible byte value there is map (c++ associative array)
//   of matched bytes with number of matches.
// For example for 0xFF there are 3 matched byte values:
// 0xEE matched 1000 times, 0xDD matched 2 times and 0xCC matched 500 times.
// In this case 0xEE value is selected for 0xFF.
// So map source-compare is 0xFF - 0xEE.

void compare_bmp_escave_outside_mode(
  const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      option::name::source_dir,
      option::name::dir_to_compare,
      option::name::output_file,
    };
    helpers::check_options(options,options_to_check);



    boost::filesystem::path source_dir =
      helpers::get_directory(
        options[option::name::source_dir].as<std::string>(),
        option::name::source_dir);
    boost::filesystem::path dir_to_compare =
      helpers::get_directory(
        options[option::name::dir_to_compare].as<std::string>(),
        option::name::dir_to_compare);

    // Have 256 elements for each possible value of source_dir byte.
    // For each element there is vector of matched bytes of dir_to_compare
    // and number of matches for each matched byte.
    std::vector<std::map<char, int>> source_compare_table =
      std::vector<std::map<char, int>>(
        tga_default_colors_num_in_pal,
        std::map<char, int>());

    for(const auto &file : boost::filesystem::directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(file.status()))
      {
        std::string bmp_bytes_source =
          helpers::read_file(
            file.path(),
            helpers::file_flag::binary | helpers::file_flag::read_all,
            0,
            4,
            helpers::read_all_dummy_size,
            option::name::source_dir);

        boost::filesystem::path file_to_compare = dir_to_compare;
        file_to_compare.append(
          file.path().filename().string(),
          boost::filesystem::path::codecvt());
        std::string bmp_bytes_to_compare =
          helpers::read_file(
            file_to_compare,
            helpers::file_flag::binary | helpers::file_flag::read_all,
            0,
            4,
            helpers::read_all_dummy_size,
            option::name::dir_to_compare);

        for(std::size_t current_byte_num = 0,
              source_file_size = bmp_bytes_source.size();
            current_byte_num <= source_file_size;
            ++current_byte_num)
        {
          std::map<char, int> &current_source_byte_map =
            source_compare_table[static_cast<std::size_t>(
              static_cast<unsigned char>(bmp_bytes_source[current_byte_num]))];
          char current_to_compare_byte =
            bmp_bytes_to_compare[current_byte_num];
          if(current_source_byte_map.count(current_to_compare_byte))
          {
            ++current_source_byte_map[current_to_compare_byte];
          }
          else
          {
            current_source_byte_map[current_to_compare_byte] = 1;
          }
        }
      }
    }



    // Getting human readable map.
    if(options[option::name::readable_output].as<bool>())
    {
      std::string compare_bytes_map_readable;
      // 256 bytes total *
      //   (2 hex of source byte +
      //    5 assumed average number of matches for each source byte *
      //      (1 space + 2 hex of compare byte + 1 ":" sign + 10 max int) +
      //    2 newline)
      compare_bytes_map_readable.reserve(
        tga_default_colors_num_in_pal * (2 + 5 * (1 + 2 + 1 + 10) + 2));

      for(std::size_t current_byte = 0;
          current_byte != tga_default_colors_num_in_pal;
          ++current_byte)
      {
        compare_bytes_map_readable.append(
          helpers::int_to_hex_string<std::size_t>(current_byte));
        for(const auto &source_byte_map : source_compare_table[current_byte])
        {
          compare_bytes_map_readable.push_back(' ');
          compare_bytes_map_readable.append(
            helpers::int_to_hex_string<char>(source_byte_map.first));
          compare_bytes_map_readable.push_back(':');
          compare_bytes_map_readable.append(
            std::to_string(source_byte_map.second));
        }
        compare_bytes_map_readable.push_back('\r');
        compare_bytes_map_readable.push_back('\n');
      }
      helpers::save_file(options[option::name::output_file].as<std::string>(),
                         compare_bytes_map_readable,
                         helpers::file_flag::binary,
                         option::name::output_file);
    }
    // getting machine readable map
    else
    {
      std::string source_compare_bytes_map;
      // 256 raw bytes total
      source_compare_bytes_map.reserve(tga_default_colors_num_in_pal);

      for(std::size_t current_byte = 0;
          current_byte != tga_default_colors_num_in_pal;
          ++current_byte)
      {
        std::map<char, int> &current_source_byte_map =
          source_compare_table[current_byte];
        if(current_source_byte_map.size() == 0)
        {
          source_compare_bytes_map.push_back('\0');
        }
        else
        {
          auto most_frequent_pair_for_source_byte =
            std::max_element(
              current_source_byte_map.begin(),
              current_source_byte_map.end(),
              [](const std::pair<char, int> &p1,
                 const std::pair<char, int> &p2)
              {
                return p1.second < p2.second;
              }
            );
          source_compare_bytes_map.push_back(
            most_frequent_pair_for_source_byte->first);
        }
      }
      helpers::save_file(options[option::name::output_file].as<std::string>(),
                         source_compare_bytes_map,
                         helpers::file_flag::binary,
                         option::name::output_file);
    }

  }
  catch(std::exception &)
  {
    std::cout << mode::name::cmp_bmp_escave_outside <<
      " mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
