#include "create_wavefront_mtl.hpp"



namespace tractor_converter{



double &mtl_color::operator[](std::size_t to_retrieve)
{
  if(to_retrieve == 0)
  {
    return r;
  }
  if(to_retrieve == 1)
  {
    return g;
  }
  if(to_retrieve == 2)
  {
    return b;
  }
  throw std::runtime_error(
    "mtl_color - " + std::to_string(to_retrieve) +
    " passed to operator[]. 0-2 expected.");
  return r;
}

const double &mtl_color::operator[](std::size_t to_retrieve) const
{
  if(to_retrieve == 0)
  {
    return r;
  }
  if(to_retrieve == 1)
  {
    return g;
  }
  if(to_retrieve == 2)
  {
    return b;
  }
  throw std::runtime_error(
    "mtl_color - " + std::to_string(to_retrieve) +
    " passed to operator[]. 0-2 expected.");
  return r;
}



mtl_color create_wavefront_mtl_helper_read_color(
  const std::string &data,
  const mat_tables::offset_pair &offset_pair)
{
  mtl_color color;
  //// VANGERS SOURCE
  //// Simplified Vangers code to get end color:
  // end_color = color_offset + I_8 >> color_shift;
  //// Where I_8 is modifier which can't be more than
  //// NORMAL define which is ((1 << 7) - 5) = 123.
  int end_offset = offset_pair.offset + (123 >> offset_pair.shift);
  if(end_offset > create_wavefront_mtl_max_end_offset)
  {
    end_offset = create_wavefront_mtl_max_end_offset;
  }
  int pos = end_offset * 3;

  for(std::size_t cur_rgb_el = 0; cur_rgb_el < 3; ++cur_rgb_el)
  {
    color[cur_rgb_el] =
      static_cast<unsigned char>(data[pos + 2 - cur_rgb_el]) / 255.0;
  }
  return color;
}



void create_wavefront_mtl_mode_helper_read_colors(
  const std::string &data,
  const mat_tables::offset_map &offset_pair_map,
  mtl_color_map &colors_map)
{
  for(const auto &offset_pair : offset_pair_map)
  {
    colors_map[offset_pair.first] =
      create_wavefront_mtl_helper_read_color(
        data,
        offset_pair.second);
  }
}



void create_wavefront_mtl_mode_helper_write_colors(
  const mtl_color_map &colors_map,
  std::string precision_mtl_str,
  std::string &data)
{
  for(const auto &color : colors_map)
  {
    data.append("newmtl " + color.first + "\n");
    data.append("Kd");
    for(std::size_t cur_rgb_el = 0; cur_rgb_el < 3 ; ++cur_rgb_el)
    {
      data.push_back(' ');
      helpers::to_string_precision<double>(color.second[cur_rgb_el],
                                           precision_mtl_str,
                                           data);
    }
    data.push_back('\n');
  }
}



void create_wavefront_mtl_mode(
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
    unsigned int wavefront_float_precision =
      options[option::name::obj_float_precision].as<unsigned int>();
    std::size_t n_wheels =
      options[option::name::mtl_n_wheels].as<std::size_t>();

    std::vector<std::string> additional_body_offsets_str =
      helpers::get_vec_str_option(options,
                                  option::name::mtl_body_offs,
                                  error_handling::none);

    mat_tables::offset_map additional_body_offsets =
      helpers::parse_mtl_body_offs(additional_body_offsets_str);



    const std::size_t expected_str_size_per_material =
      // "newmtl body_offset_144_shift_3" + 2 newline
      30 + 2 +
      // 2 "Kd" + 3 numbers * (1 space + 1 digit + 1 dot + precision) +
      // 2 newline
      2 + 3 * (1 + 1 + 1 + wavefront_float_precision) +
      2;



    for(const auto &file : boost::filesystem::directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(file.status()) &&
         file.path().extension().string() == ext::pal)
      {
        std::string source_pal =
          helpers::read_file(file.path(),
                             helpers::file_flag::binary,
                             0,
                             0,
                             tga_default_pal_size,
                             option::name::source_dir);


        std::vector<std::string> mtl_append_order =
          mat_tables::mtl::append_order;

        std::unordered_map<std::string, mtl_color_map> colors_maps;

        create_wavefront_mtl_mode_helper_read_colors(
          source_pal,
          mat_tables::regular_offsets,
          colors_maps[mat_tables::append_order_el::regular]);
        create_wavefront_mtl_mode_helper_read_colors(
          source_pal,
          mat_tables::special_offsets,
          colors_maps[mat_tables::append_order_el::special]);
        create_wavefront_mtl_mode_helper_read_colors(
          source_pal,
          mat_tables::default_body_offsets,
          colors_maps[mat_tables::append_order_el::default_body]);
        create_wavefront_mtl_mode_helper_read_colors(
          source_pal,
          additional_body_offsets,
          colors_maps[mat_tables::append_order_el::additional_body]);

        for(const auto &cur_base : mat_tables::mtl::append_order)
        {
          for(const auto &wheel_marker : wheel_markers)
          {
            std::string cur_colors_maps_entry = cur_base + wheel_marker;
            mtl_append_order.push_back(cur_colors_maps_entry);
            for(const auto &name_mat_pair : colors_maps[cur_base])
            {
              for(std::size_t cur_wheel = 0; cur_wheel < n_wheels; ++cur_wheel)
              {
                colors_maps[cur_colors_maps_entry]
                    [name_mat_pair.first + wheel_marker +
                     wavefront_obj::mat_separator +
                     std::to_string(cur_wheel + 1)] =
                  name_mat_pair.second;
              }
            }
          }
        }

        for(const auto &cur_base : mat_tables::mtl::append_order)
        {
          std::string cur_colors_maps_entry =
            cur_base + wavefront_obj::weapon_mat_marker;
          mtl_append_order.push_back(cur_colors_maps_entry);
          for(const auto &name_mat_pair : colors_maps[cur_base])
          {
            for(std::size_t cur_slot = 0;
                cur_slot < m3d::weapon_slot::max_slots;
                ++cur_slot)
            {
              colors_maps[cur_colors_maps_entry]
                  [name_mat_pair.first + wavefront_obj::weapon_mat_marker +
                   wavefront_obj::mat_separator +
                   std::to_string(cur_slot + 1)] =
                name_mat_pair.second;
            }
          }
        }



        std::string mtl_file;
        // Counting number of materials to be written to *.mtl file.
        std::size_t mat_to_write = 0;
        for(const auto &cur_append : mtl_append_order)
        {
          for(const auto &color : colors_maps[cur_append])
          {
            ++mat_to_write;
          }
        }
        mtl_file.reserve(expected_str_size_per_material * mat_to_write);


        std::string precision_mtl_str =
          "%." + std::to_string(wavefront_float_precision) + "f";



        for(const auto &cur_append : mtl_append_order)
        {
          create_wavefront_mtl_mode_helper_write_colors(
            colors_maps[cur_append],
            precision_mtl_str,
            mtl_file);
        }



        boost::filesystem::path file_to_save = output_dir;
        file_to_save.append(file.path().stem().string() + ext::mtl,
                            boost::filesystem::path::codecvt());
        helpers::save_file(file_to_save,
                           mtl_file,
                           helpers::file_flag::none,
                           option::name::output_dir);
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << mode::name::create_wavefront_mtl << " mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
