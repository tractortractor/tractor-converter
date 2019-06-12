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
  const c3d::color::offset_pair &offset_pair)
{
  mtl_color color;
  // VANGERS SOURCE
  // Simplified Vangers code to get end color:
  // end_color = color_offset + I_8 >> color_shift;
  // Where I_8 is modifier which can't be more than
  // NORMAL define which is ((1 << 7) - 5) = 123.
  int end_offset = offset_pair.offset + (123 >> offset_pair.shift);
  if(end_offset > create_wavefront_mtl_max_end_offset)
  {
    end_offset = create_wavefront_mtl_max_end_offset;
  }
  int pos = end_offset * 3;

  // TEST
//std::cout << '\n';
//std::cout << "offset_pair.offset: " <<
//  std::to_string(offset_pair.offset) << '\n';
//std::cout << "offset_pair.shift: " <<
//  std::to_string(offset_pair.shift) << '\n';
//std::cout << "end_offset: " << end_offset << '\n';
//std::cout << "end_pos: " << std::to_string(pos) << '\n';

  for(std::size_t cur_rgb_el = 0; cur_rgb_el < 3; ++cur_rgb_el)
  {
    color[cur_rgb_el] =
      static_cast<unsigned char>(data[pos + 2 - cur_rgb_el]) / 255.0;

  // TEST
//  std::cout << "color[" << cur_rgb_el << "]" << color[cur_rgb_el] << '\n';
  }
  return color;
}



void create_wavefront_mtl_mode_helper_read_colors(
  const std::string &data,
  const c3d::color::offset_map &offset_pair_map,
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
      "source_dir",
      "output_dir"
    };
    helpers::check_options(options, options_to_check);



    boost::filesystem::path source_dir =
      helpers::get_directory(options["source_dir"].as<std::string>(),
                             "source_dir");
    boost::filesystem::path output_dir =
      helpers::get_directory(options["output_dir"].as<std::string>(),
                             "output_dir");
    int wavefront_float_precision =
      options["3d_obj_float_precision"].as<int>();
    std::size_t n_wheels =
      options["mtl_n_wheels"].as<std::size_t>();

    std::vector<std::string> additional_body_offsets_str =
      helpers::get_vec_str_option(options,
                                  "mtl_body_offs",
                                  error_handling::none);

    c3d::color::offset_map additional_body_offsets =
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
         file.path().extension().string() == ".pal")
      {
        std::string source_pal =
          helpers::read_file(file.path(),
                             helpers::file_flag::binary,
                             0,
                             0,
                             tga_default_pal_size,
                             "source_dir");



        std::unordered_map<std::string, mtl_color_map> colors_maps;

        create_wavefront_mtl_mode_helper_read_colors(
          source_pal,
          c3d::color::offsets,
          colors_maps["regular"]);
        create_wavefront_mtl_mode_helper_read_colors(
          source_pal,
          c3d::color::default_body_offsets,
          colors_maps["default_body"]);

        create_wavefront_mtl_mode_helper_read_colors(
          source_pal,
          additional_body_offsets,
          colors_maps["additional_body"]);

        mtl_color wheel_color =
          create_wavefront_mtl_helper_read_color(
            source_pal,
            c3d::color::wheel_offset);
        mtl_color weapon_color =
          create_wavefront_mtl_helper_read_color(
            source_pal,
            c3d::color::weapon_offset);
        mtl_color attachment_point_color =
          create_wavefront_mtl_helper_read_color(
            source_pal,
            c3d::color::attachment_point_offset);
        mtl_color center_of_mass_color =
          create_wavefront_mtl_helper_read_color(
            source_pal,
            c3d::color::center_of_mass_offset);

        colors_maps["wheel"]["wheel"] = wheel_color;
        colors_maps["wheel_steer"]["wheel_steer"] = wheel_color;
        colors_maps["wheel_ghost"]["wheel_ghost"] = wheel_color;
        colors_maps["wheel_steer_ghost"]["wheel_steer_ghost"] =
          wheel_color;
        colors_maps["wheel_ghost_steer"]["wheel_ghost_steer"] =
          wheel_color;
        for(std::size_t cur_wheel = 0; cur_wheel < n_wheels; ++cur_wheel)
        {
          colors_maps["wheel"]
              ["wheel_" + std::to_string(cur_wheel + 1)] =
            wheel_color;
          colors_maps["wheel_steer"]
              ["wheel_steer_" + std::to_string(cur_wheel + 1)] =
            wheel_color;
          colors_maps["wheel_ghost"]
              ["wheel_ghost_" + std::to_string(cur_wheel + 1)] =
            wheel_color;
          colors_maps["wheel_steer_ghost"]
              ["wheel_steer_ghost_" + std::to_string(cur_wheel + 1)] =
            wheel_color;
          colors_maps["wheel_ghost_steer"]
              ["wheel_ghost_steer_" + std::to_string(cur_wheel + 1)] =
            wheel_color;
        }

        colors_maps["weapon"]["weapon"] = weapon_color;
        for(std::size_t cur_slot = 0;
            cur_slot < m3d::weapon_slot::max_slots;
            ++cur_slot)
        {
          colors_maps["weapon"]["weapon_" + std::to_string(cur_slot + 1)] =
            weapon_color;
        }

        colors_maps["attachment_point"]["attachment_point"] =
          attachment_point_color;
        for(std::size_t cur_slot = 0;
            cur_slot < m3d::weapon_slot::max_slots;
            ++cur_slot)
        {
          colors_maps["attachment_point"]
              ["attachment_point_" + std::to_string(cur_slot + 1)] =
            attachment_point_color;
        }

        colors_maps["center_of_mass"]["center_of_mass"] = center_of_mass_color;



        const std::vector<std::string> mtl_append_order
        {
          "regular",
          "center_of_mass",
          "weapon",
          "attachment_point",
          "default_body",
          "additional_body",
          "wheel",
          "wheel_steer",
          "wheel_ghost",
          "wheel_steer_ghost",
          "wheel_ghost_steer",
        };

        std::string mtl_file;
        // Counting number of materials to be written in *.mtl file.
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
        file_to_save.append(file.path().stem().string() + ".mtl",
                            boost::filesystem::path::codecvt());
        helpers::save_file(file_to_save,
                           mtl_file,
                           helpers::file_flag::none,
                           "output_dir");
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << "create_wavefront_mtl mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
