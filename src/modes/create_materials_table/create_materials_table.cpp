#include "create_materials_table.hpp"



namespace tractor_converter{



unsigned char &html_color::operator[](std::size_t to_retrieve)
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
    "html_color - " + std::to_string(to_retrieve) +
    " passed to operator[]. 0-2 expected.");
  return r;
}

const unsigned char &html_color::operator[](std::size_t to_retrieve) const
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
    "html_color - " + std::to_string(to_retrieve) +
    " passed to operator[]. 0-2 expected.");
  return r;
}



html_material::html_material()
{
}

html_material::html_material(std::vector<html_color> colors_arg,
                             int offset_arg,
                             int shift_arg,
                             html_material_changes changes_arg)
: offset_pair(offset_arg, shift_arg),
  colors(colors_arg),
  changes(changes_arg)
{
}

html_material::html_material(std::vector<html_color> colors_arg,
                             mat_tables::offset_pair pair_arg,
                             html_material_changes changes_arg)
: offset_pair(pair_arg),
  colors(colors_arg),
  changes(changes_arg)
{
}



html_material create_materials_table_mode_helper_read_material(
  const std::string &data,
  const mat_tables::offset_pair &offset_pair)
{
  //// VANGERS SOURCE
  //// Simplified Vangers code to get end color:
  // end_color = color_offset + I_8 >> color_shift;
  //// Where I_8 is modifier which can't be more than
  //// NORMAL define which is ((1 << 7) - 5) = 123.
  int end_offset = offset_pair.offset + (123 >> offset_pair.shift);
  if(end_offset > create_materials_table_max_end_offset)
  {
    end_offset = create_materials_table_max_end_offset;
  }
  int pos = offset_pair.offset * 3;
  int end_pos = end_offset * 3;

  std::vector<html_color> colors;
  for(; pos <= end_pos; pos = pos + 3)
  {
    html_color color;

    for(std::size_t cur_rgb_el = 0; cur_rgb_el < 3; ++cur_rgb_el)
    {
      color[cur_rgb_el] = data[pos + 2 - cur_rgb_el];
    }
    colors.push_back(color);
  }
  html_material_changes changes = html_material_changes::none;
  // If color_ind_changes_per_world_range intersects offset, end_offset range.
  if(color_ind_changes_per_world_range.first <= end_offset &&
     color_ind_changes_per_world_range.second >= offset_pair.offset)
  {
    changes = html_material_changes::per_world;
  }
  // If color_ind_changes_per_quant_range intersects offset, end_offset range.
  if(color_ind_changes_per_quant_range.first <= end_offset &&
     color_ind_changes_per_quant_range.second >= offset_pair.offset)
  {
    changes = html_material_changes::per_quant;
  }
  return html_material(colors, offset_pair, changes);
}



void create_materials_table_mode_helper_read_materials(
  const std::string &data,
  const mat_tables::offset_map &offset_pair_map,
  html_material_map &materials_map)
{
  for(const auto &offset_pair : offset_pair_map)
  {
    materials_map[offset_pair.first] =
      create_materials_table_mode_helper_read_material(
        data,
        offset_pair.second);
  }
}



void create_materials_table_mode_helper_write_materials(
  const html_material_map &materials_map,
  std::string &data)
{
  for(const auto &material : materials_map)
  {

    data.append("  <tr>\n");
    data.append("    <td>" + material.first + "</td>\n");
    data.append(
      "    <td>" + std::to_string(material.second.offset) + "</td>\n");
    data.append(
      "    <td>" + std::to_string(material.second.shift) + "</td>\n");

    data.append("    <td>");
    if(material.second.changes == html_material_changes::none)
    {
      data.append(html_table_changes_none);
    }
    else if(material.second.changes == html_material_changes::per_world)
    {
      data.append(html_table_changes_per_world);
    }
    else if(material.second.changes == html_material_changes::per_quant)
    {
      data.append(html_table_changes_per_quant);
    }
    data.append("</td>\n");

    for(const auto &color : material.second.colors)
    {
      data.append(
        "    <td><div style=\"background-color:#");

      for(std::size_t cur_rgb_el = 0; cur_rgb_el < 3; ++cur_rgb_el)
      {
        std::string tmp = helpers::int_to_hex_string<char>(color[cur_rgb_el]);
        if(tmp.size() == 1)
        {
          tmp = "0" + tmp;
        }
        data.append(tmp);
      }
      data.append("; height:2ex; width:2ex\"></div></td>\n");
    }

    data.append("  </tr>\n");
  }
}



void create_materials_table_mode(
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

    std::vector<std::string> additional_body_offsets_str =
      helpers::get_vec_str_option(options,
                                  option::name::mtl_body_offs,
                                  error_handling::none);

    mat_tables::offset_map additional_body_offsets =
      helpers::parse_mtl_body_offs(additional_body_offsets_str);



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



        std::unordered_map<std::string, html_material_map> materials_maps;

        create_materials_table_mode_helper_read_materials(
          source_pal,
          mat_tables::regular_offsets,
          materials_maps[mat_tables::append_order_el::regular]);

        create_materials_table_mode_helper_read_materials(
          source_pal,
          mat_tables::default_body_offsets,
          materials_maps[mat_tables::append_order_el::default_body]);

        create_materials_table_mode_helper_read_materials(
          source_pal,
          additional_body_offsets,
          materials_maps[mat_tables::append_order_el::additional_body]);

        std::string html_table_file;
        // Counting number of materials to be written in *.html file.
        std::size_t mat_to_write = 0;
        std::size_t colors_to_write = 0;
        for(const auto &cur_append : mat_tables::html::append_order)
        {
          for(const auto &material : materials_maps[cur_append])
          {
            ++mat_to_write;
            for(const auto &color : material.second.colors)
            {
              ++colors_to_write;
            }
          }
        }
        html_table_file.reserve(
          html_table_header_size +
          expected_size_per_material * mat_to_write +
          expected_size_per_material_color * colors_to_write +
          html_table_footer_size);

        html_table_file.append(html_table_header);
        for(const auto &cur_append : mat_tables::html::append_order)
        {
          create_materials_table_mode_helper_write_materials(
            materials_maps[cur_append],
            html_table_file);
        }
        html_table_file.append(html_table_footer);



        boost::filesystem::path file_to_save = output_dir;
        file_to_save.append(file.path().stem().string() + ext::html,
                            boost::filesystem::path::codecvt());
        helpers::save_file(file_to_save,
                           html_table_file,
                           helpers::file_flag::none,
                           option::name::output_dir);
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << mode::name::create_materials_table << " mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
