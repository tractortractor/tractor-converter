#include "wavefront_obj_operations.hpp"



namespace tractor_converter{
namespace helpers{



int raw_obj_to_volInt_helper_get_wheel_weapon_id(
  std::string mat_name,
  std::size_t mat_size)
{
  std::size_t mat_size_with_separator =
    mat_size + wavefront_obj::mat_separator_size;

  if(mat_name.size() >= mat_size_with_separator &&
     !std::strncmp(mat_name.c_str() + mat_size,
                   wavefront_obj::mat_separator.c_str(),
                   wavefront_obj::mat_separator_size))
  {
    std::string tmp = mat_name.substr(mat_size_with_separator);
    try
    {
      return std::stoi(tmp) - 1;
    }
    // If tmp does not contain number.
    catch(std::invalid_argument &)
    {
    }
  }
  return -1;
}



void raw_obj_to_volInt_helper_get_body_color(
  const boost::filesystem::path &input_file_path_arg,
  const std::string &input_file_name_error,
  std::string mat_name,
  volInt::polyhedron &volInt_model)
{
  // Checking if string is "body_offset_*_shift_*" string. * - integer.
  // Skipping color operations if it doesn't match.

  int new_color_offset = -1;
  int new_color_shift = -1;

  // Size of first integer.
  std::size_t offset_num_size;
  // Size of "body_offset_*_shift_".
  std::size_t body_mat_pre_shift_number_size;

  std::string color_offset_str;
  std::string color_shift_str;
  // Checking if there is enough space for "body_offset_*".
  if(mat_name.size() < wavefront_obj::body_mat_str_el_1_size)
  {
    return;
  }
  color_offset_str =
    mat_name.substr(wavefront_obj::body_mat_str_el_1_size);
  try
  {
    new_color_offset = std::stoi(color_offset_str, &offset_num_size);
  }
  // If there is no integer in place of * in "body_offset_*".
  catch(std::invalid_argument &)
  {
    return;
  }
  // Size of "body_offset_*_shift_".
  body_mat_pre_shift_number_size =
    wavefront_obj::body_mat_str_el_1_size +
    offset_num_size +
     wavefront_obj::body_mat_str_el_2_size;
  if(// Checking if there is enough space for "body_offset_*_shift_*".
     mat_name.size() < body_mat_pre_shift_number_size ||
     // Checking if there is "_shift_" part of string.
     std::strncmp(mat_name.c_str() +
                    wavefront_obj::body_mat_str_el_1_size +
                    offset_num_size,
                  wavefront_obj::body_mat_str_el_2.c_str(),
                  wavefront_obj::body_mat_str_el_2_size))
  {
    return;
  }
  color_shift_str =
    mat_name.substr(body_mat_pre_shift_number_size);
  try
  {
    new_color_shift =
      std::stoi(color_shift_str);
  }
  // If mat_name does not contain color_shift number.
  catch(std::invalid_argument &)
  {
    return;
  }


  if(volInt_model.bodyColorOffset != -1 &&
     (volInt_model.bodyColorOffset != new_color_offset ||
        volInt_model.bodyColorShift != new_color_shift))
  {
    throw std::runtime_error(
      "Error while loading " + input_file_name_error +
      " file " + input_file_path_arg.string() +
      " as wavefront *.obj. Several body colors detected: offset " +
      std::to_string(volInt_model.bodyColorOffset) + ", shift " +
      std::to_string(volInt_model.bodyColorShift) + " and offset " +
      std::to_string(new_color_offset) + ", shift " +
      std::to_string(new_color_shift) +
      ". Only one unique body color is allowed per *.m3d model.");
  }
  volInt_model.bodyColorOffset = new_color_offset;
  volInt_model.bodyColorShift = new_color_shift;

  if(volInt_model.bodyColorOffset > c3d::color::max_offset ||
     volInt_model.bodyColorShift > c3d::color::max_shift ||
     volInt_model.bodyColorOffset < 0 ||
     volInt_model.bodyColorShift < 0)
  {
    throw std::runtime_error(
      "Error while loading " + input_file_name_error +
      " file " + input_file_path_arg.string() +
      " as wavefront *.obj. Body color offset " +
      std::to_string(volInt_model.bodyColorOffset) +
      " or body color shift " +
      std::to_string(volInt_model.bodyColorShift) +
      " is out of range." +
      " Color offset range: 0 - " +
      std::to_string(c3d::color::max_offset) + "." +
      " Color shift range: 0 - " +
      std::to_string(c3d::color::max_shift) + ".");
  }
}



void raw_obj_to_volInt_helper_get_wheel_properties(
  const boost::filesystem::path &input_file_path_arg,
  const std::string &input_file_name_error,
  std::size_t mat,
  std::string mat_name,
  std::vector<int> &tiny_obj_id_to_wheel_weapon_id,
  volInt::polyhedron &volInt_model)
{
  std::size_t mat_size = wavefront_obj::wheel_mat_str_size;

  bool steer = false;
  bool ghost = false;

  if(!std::strncmp(mat_name.c_str() + mat_size,
                   wavefront_obj::wheel_steer_mat_str_el.c_str(),
                   wavefront_obj::wheel_steer_mat_str_el_size))
  {
    mat_size += wavefront_obj::wheel_steer_mat_str_el_size;
    steer = true;
  }

  if(!std::strncmp(mat_name.c_str() + mat_size,
                   wavefront_obj::wheel_ghost_mat_str_el.c_str(),
                   wavefront_obj::wheel_ghost_mat_str_el_size))
  {
    mat_size += wavefront_obj::wheel_ghost_mat_str_el_size;
    ghost = true;
  }

  // Second check in case "steer" modifier goes after "ghost".
  if(!steer)
  {
    if(!std::strncmp(mat_name.c_str() + mat_size,
                     wavefront_obj::wheel_steer_mat_str_el.c_str(),
                     wavefront_obj::wheel_steer_mat_str_el_size))
    {
      mat_size += wavefront_obj::wheel_steer_mat_str_el_size;
      steer = true;
    }
  }

  int cur_wheel_id =
    raw_obj_to_volInt_helper_get_wheel_weapon_id(mat_name,
                                                 mat_size);
  if(cur_wheel_id >= 0)
  {
    tiny_obj_id_to_wheel_weapon_id[mat] = cur_wheel_id;

    volInt_model.wheels.insert(cur_wheel_id);

    if(steer)
    {
      volInt_model.wheels_steer.insert(cur_wheel_id);
    }
    else
    {
      volInt_model.wheels_non_steer.insert(cur_wheel_id);
    }

    if(ghost)
    {
      volInt_model.wheels_ghost.insert(cur_wheel_id);
    }
    else
    {
      volInt_model.wheels_non_ghost.insert(cur_wheel_id);
    }

    if(volInt_model.wheels_steer.count(cur_wheel_id) &&
       volInt_model.wheels_non_steer.count(cur_wheel_id))
    {
      throw std::runtime_error(
        "Error while loading " + input_file_name_error +
        " file " + input_file_path_arg.string() +
        " as wavefront *.obj. " +
        "There are steer and non-steer materials of wheel " +
        std::to_string(cur_wheel_id) +
        ". Wheel must be either steer or non-steer.");
    }

    if(volInt_model.wheels_ghost.count(cur_wheel_id) &&
       volInt_model.wheels_non_ghost.count(cur_wheel_id))
    {
      throw std::runtime_error(
        "Error while loading " + input_file_name_error +
        " file " + input_file_path_arg.string() +
        " as wavefront *.obj. " +
        "There are ghost and non-ghost materials of wheel " +
        std::to_string(cur_wheel_id) +
        ". Wheel must be either ghost or non-ghost.");
    }
  }
}



void raw_obj_to_volInt_helper_get_weapon_num(
  const boost::filesystem::path &input_file_path_arg,
  const std::string &input_file_name_error,
  std::size_t mat,
  std::string mat_name,
  std::vector<int> &tiny_obj_id_to_wheel_weapon_id)
{
  tiny_obj_id_to_wheel_weapon_id[mat] =
    raw_obj_to_volInt_helper_get_wheel_weapon_id(
      mat_name,
      wavefront_obj::weapon_mat_str_size);
  if(tiny_obj_id_to_wheel_weapon_id[mat] >=
     static_cast<int>(m3d::weapon_slot::max_slots))
  {
    throw std::runtime_error(
      "Error while loading " + input_file_name_error +
      " file " + input_file_path_arg.string() +
      " as wavefront *.obj. ID of weapon material " +
      std::to_string(tiny_obj_id_to_wheel_weapon_id[mat]) +
      " is more than max weapon slot " +
      std::to_string(m3d::weapon_slot::max_slots) + ".");
  }
}



void raw_obj_to_volInt_helper_get_attachment_point_num(
  const boost::filesystem::path &input_file_path_arg,
  const std::string &input_file_name_error,
  std::size_t mat,
  std::string mat_name,
  std::vector<int> &tiny_obj_id_to_wheel_weapon_id)
{
  tiny_obj_id_to_wheel_weapon_id[mat] =
    raw_obj_to_volInt_helper_get_wheel_weapon_id(
      mat_name,
      wavefront_obj::attachment_point_mat_str_size);
  if(tiny_obj_id_to_wheel_weapon_id[mat] >=
     static_cast<int>(m3d::weapon_slot::max_slots))
  {
    throw std::runtime_error(
      "Error while loading " + input_file_name_error +
      " file " + input_file_path_arg.string() +
      " as wavefront *.obj. ID of attachment point material " +
      std::to_string(tiny_obj_id_to_wheel_weapon_id[mat]) +
      " is more than max weapon slot " +
      std::to_string(m3d::weapon_slot::max_slots) + ".");
  }
}



volInt::polyhedron raw_obj_to_volInt_model(
  const boost::filesystem::path &input_file_path_arg,
  const std::string &input_file_name_error,
  c3d::c3d_type type,
  int default_color_id)
{
  std::string obj_data =
    read_file(input_file_path_arg,
              file_flag::read_all,
              0,
              0,
              read_all_dummy_size,
              input_file_name_error);


  // Reading *.obj file with tiny_obj_loader

  unsigned char expected_n_verts_per_poly;
  std::string expected_n_verts_per_poly_err_str;
  if(type == c3d::c3d_type::regular)
  {
    expected_n_verts_per_poly = c3d::regular_model_vertices_per_polygon;
    expected_n_verts_per_poly_err_str = "regular";
  }
  if(type == c3d::c3d_type::bound)
  {
    expected_n_verts_per_poly = c3d::bound_model_vertices_per_polygon;
    expected_n_verts_per_poly_err_str = "bound";
  }

  std::stringstream objStream(obj_data);

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;
  bool ret = tinyobj::LoadObj(&attrib,
                              &shapes,
                              &materials,
                              &err,
                              &objStream,
                              nullptr,
                              false,
                              false);

  if(!err.empty())
  {
    throw std::runtime_error(
      "Error while loading " + input_file_name_error +
      " file " + input_file_path_arg.string() +
      " as wavefront *.obj. tiny_obj_loader error: " + err);
  }

  if(!ret)
  {
    throw std::runtime_error(
      "Error while loading " + input_file_name_error +
      " file " + input_file_path_arg.string() +
      " as wavefront *.obj. tinyobj::LoadObj returned false.");
  }





  // Getting sizes and checking if retrieved info is correct.

  // attrib.vertices and attrib.norms are std::vector<double>.
  // Each 3 double numbers = 1 vertex.
  int num_verts = attrib.vertices.size() / volInt::axes_num;
  int num_vert_norms = attrib.normals.size() / volInt::axes_num;

  int num_faces = 0;
  for(const auto &shape : shapes)
  {
    num_faces += shape.mesh.num_face_vertices.size();
  }



  for(const auto &shape : shapes)
  {
    for(const auto &num_face_vertex : shape.mesh.num_face_vertices)
    {
      if(expected_n_verts_per_poly != num_face_vertex)
      {
        throw std::runtime_error(
          "Error while loading " + input_file_name_error +
          " file " + input_file_path_arg.string() +
          " as wavefront *.obj. Expected " +
          expected_n_verts_per_poly_err_str +
          " object with " +
          std::to_string(
            static_cast<unsigned short int>(expected_n_verts_per_poly)) +
          " vertices per polygon. Found polygon with " +
          std::to_string(static_cast<unsigned short int>(num_face_vertex)) +
          " vertices.");
      }
    }
  }





  volInt::polyhedron volInt_model(num_verts,
                                  num_vert_norms,
                                  num_faces,
                                  expected_n_verts_per_poly);
  volInt_model.wavefront_obj_path = input_file_path_arg.string();

  std::size_t materials_size = materials.size();
  std::vector<unsigned int> tiny_obj_id_to_color_id(materials_size,
                                                    default_color_id);
  std::vector<int> tiny_obj_id_to_wheel_weapon_id(materials_size, -1);

  for(std::size_t cur_mat = 0; cur_mat < materials_size; ++cur_mat)
  {
    const std::string &cur_mat_name = materials[cur_mat].name;
    // Checking if first part of cur_mat_name is "body_offset_".
    if(!std::strncmp(cur_mat_name.c_str(),
                     wavefront_obj::body_mat_str_el_1.c_str(),
                     wavefront_obj::body_mat_str_el_1_size))
    {
      // Assigning color ID.
      tiny_obj_id_to_color_id[cur_mat] = c3d::color::string_to_id::body;

      raw_obj_to_volInt_helper_get_body_color(
        input_file_path_arg,
        input_file_name_error,
        cur_mat_name,
        volInt_model);
    }
    else if(!std::strncmp(cur_mat_name.c_str(),
                          wavefront_obj::wheel_mat_str.c_str(),
                          wavefront_obj::wheel_mat_str_size))
    {
      // Assigning color ID.
      tiny_obj_id_to_color_id[cur_mat] = c3d::color::string_to_id::wheel;

      raw_obj_to_volInt_helper_get_wheel_properties(
        input_file_path_arg,
        input_file_name_error,
        cur_mat,
        cur_mat_name,
        tiny_obj_id_to_wheel_weapon_id,
        volInt_model);
    }
    else if(!std::strncmp(cur_mat_name.c_str(),
                          wavefront_obj::weapon_mat_str.c_str(),
                          wavefront_obj::weapon_mat_str_size))
    {
      // Assigning color ID.
      tiny_obj_id_to_color_id[cur_mat] = c3d::color::string_to_id::weapon;

      raw_obj_to_volInt_helper_get_weapon_num(
        input_file_path_arg,
        input_file_name_error,
        cur_mat,
        cur_mat_name,
        tiny_obj_id_to_wheel_weapon_id);
    }
    else if(!std::strncmp(cur_mat_name.c_str(),
                          wavefront_obj::attachment_point_mat_str.c_str(),
                          wavefront_obj::attachment_point_mat_str_size))
    {
      // Assigning color ID.
      tiny_obj_id_to_color_id[cur_mat] =
        c3d::color::string_to_id::attachment_point;

      raw_obj_to_volInt_helper_get_attachment_point_num(
        input_file_path_arg,
        input_file_name_error,
        cur_mat,
        cur_mat_name,
        tiny_obj_id_to_wheel_weapon_id);
    }
    else
//  else if(c3d::color::ids.by<c3d::color::name>().count(cur_mat_name))
    {
      // Doing max_name_size check.
      // So longer material names will be selected.
      // Example: "body_green" vs "body", "body_green" will be selected.
      std::size_t max_name_size = 0;
      for(const auto &name_id_pair : c3d::color::ids.by<c3d::color::name>())
      {
        std::size_t name_size = name_id_pair.first.size();
        if(max_name_size < name_size &&
           !std::strncmp(cur_mat_name.c_str(),
                         name_id_pair.first.c_str(),
                         name_size))
        {
          // Assigning color ID.
          tiny_obj_id_to_color_id[cur_mat] = name_id_pair.second;
//        tiny_obj_id_to_color_id[cur_mat] =
//          c3d::color::ids.by<c3d::color::name>().at(cur_mat_name);
          max_name_size = name_id_pair.first.size();
        }
      }
    }
//  else
//  {
//    // Assigning color ID.
//    tiny_obj_id_to_color_id[cur_mat] = default_color_id;
//  }
  }



  if(volInt_model.bodyColorOffset > c3d::color::max_offset ||
     volInt_model.bodyColorShift  > c3d::color::max_shift ||
     volInt_model.bodyColorOffset < 0 ||
     volInt_model.bodyColorShift  < 0)
  {
    volInt_model.bodyColorOffset = c3d::color::default_offset;
    volInt_model.bodyColorShift  = c3d::color::default_shift;
  }



  // Checking wheels
  int n_wheels = volInt_model.wheels.size();
  std::vector<int> missed_wheels;
  std::vector<int> out_of_range_wheels;
  missed_wheels.reserve(n_wheels);
  out_of_range_wheels.reserve(n_wheels);
  for(int cur_wheel = 0; cur_wheel < n_wheels; ++cur_wheel)
  {
    if(!volInt_model.wheels.count(cur_wheel))
    {
      missed_wheels.push_back(cur_wheel);
    }
  }
  for(int cur_wheel : volInt_model.wheels)
  {
    if(cur_wheel >= n_wheels)
    {
      out_of_range_wheels.push_back(cur_wheel);
    }
  }

  if(missed_wheels.size() || out_of_range_wheels.size())
  {
    std::string missed_wheels_str;
    std::string out_of_range_wheels_str;
    for(int missed_wheel : missed_wheels)
    {
      missed_wheels_str.append(std::to_string(missed_wheel + 1) + "\n");
    }
    for(int out_of_range_wheel : out_of_range_wheels)
    {
      out_of_range_wheels_str.append(
        std::to_string(out_of_range_wheel + 1) + "\n");
    }
    throw std::runtime_error(
      "In " + input_file_name_error + " file " +
      input_file_path_arg.string() +
      " wheels order is wrong. " + '\n' +
      "It is assumed that all wheels are numbered " +
      "from 1 to number of wheels." + '\n' +
      "Missed wheels:" + '\n' +
      missed_wheels_str +
      "Out of range wheels: " + '\n' +
      out_of_range_wheels_str +
      '\n');
  }





  // Writing data to volInt::polyhedron.
  std::size_t cur_vert_pos = 0;
  for(int cur_vertex = 0;
      cur_vertex < num_verts;
      ++cur_vertex)
  {
    for(std::size_t coord_el = 0; coord_el < volInt::axes_num; ++coord_el)
    {
      volInt_model.verts[cur_vertex][coord_el] = attrib.vertices[cur_vert_pos];
      ++cur_vert_pos;
    }
  }

  std::size_t cur_vert_norm_pos = 0;
  for(int cur_vert_normal = 0;
      cur_vert_normal < num_vert_norms;
      ++cur_vert_normal)
  {
    for(std::size_t coord_el = 0; coord_el < volInt::axes_num; ++coord_el)
    {
      volInt_model.vertNorms[cur_vert_normal][coord_el] =
        attrib.normals[cur_vert_norm_pos];
      ++cur_vert_norm_pos;
    }
  }

  std::size_t cur_volint_poly_num = 0;
  for(const auto &shape : shapes)
  {
    std::size_t cur_poly_pos = 0;
    for(std::size_t cur_poly_num = 0,
          poly_size = shape.mesh.num_face_vertices.size();
        cur_poly_num < poly_size;
        ++cur_poly_num)
    {
      volInt::face &cur_volint_poly_ref =
        volInt_model.faces[cur_volint_poly_num];
      ++cur_volint_poly_num;
      for(std::size_t cur_poly_vert_num = 0;
          cur_poly_vert_num < expected_n_verts_per_poly;
          ++cur_poly_vert_num)
      {
        const tinyobj::index_t &cur_tinyobj_index_ref =
          shape.mesh.indices[cur_poly_pos];
        ++cur_poly_pos;
        cur_volint_poly_ref.verts[cur_poly_vert_num] =
          cur_tinyobj_index_ref.vertex_index;
        cur_volint_poly_ref.vertNorms[cur_poly_vert_num] =
          cur_tinyobj_index_ref.normal_index;
      }
      int tiny_obj_mat_id = shape.mesh.material_ids[cur_poly_num];

      if(tiny_obj_mat_id < 0)
      {
        cur_volint_poly_ref.color_id = default_color_id;
        cur_volint_poly_ref.wheel_weapon_id = -1;
      }
      else
      {
        cur_volint_poly_ref.color_id =
          tiny_obj_id_to_color_id[tiny_obj_mat_id];
        cur_volint_poly_ref.wheel_weapon_id =
          tiny_obj_id_to_wheel_weapon_id[tiny_obj_mat_id];
      }



      // If polygon is saved in actual *.m3d/*.a3d model
      // check if normal indices were specified in *.obj file.
      if(cur_volint_poly_ref.color_id <
         c3d::color::string_to_id::max_colors_ids)
      {
        for(std::size_t cur_poly_vert_num = 0;
            cur_poly_vert_num < expected_n_verts_per_poly;
            ++cur_poly_vert_num)
        {
          if(cur_volint_poly_ref.vertNorms[cur_poly_vert_num] < 0)
          {
            throw std::runtime_error(
              "In " + input_file_name_error + " file " +
              input_file_path_arg.string() +
              " face " + std::to_string(cur_poly_num) +
              " have no normal index specified for vertex " +
              std::to_string(cur_poly_vert_num) + "." + '\n' +
              "All faces which are part of actual model" +
              " must have specified normal indices.");
          }
        }
      }
    }
  }

  // TEST
//std::cout << "wavefront_obj_operations reading " <<
//  input_file_path_arg << '\n';



  volInt_model.faces_calc_params();

  return volInt_model;
}





void save_volInt_as_wavefront_obj(
  const std::unordered_map<std::string, volInt::polyhedron> &c3d_models,
  const boost::filesystem::path &output_path,
  const std::string &output_file_name_error,
  const std::string &float_precision_objs_string,
  std::size_t expected_medium_vertex_size,
  std::size_t expected_medium_normal_size)
{
//std::size_t cur_wheel_data_size = cur_wheel_data.size();

  std::string obj_data;

  std::size_t to_reserve = wavefront_obj::mtl_reference_size;
  for(const auto &c3d_model : c3d_models)
  {
    to_reserve +=
      wavefront_obj::base_obj_name_size + c3d_model.first.size() +
      c3d_model.second.verts.size() * expected_medium_vertex_size +
      c3d_model.second.vertNorms.size() * expected_medium_normal_size +
      c3d_model.second.faces.size() * expected_polygon_size;
  }
  obj_data.reserve(to_reserve);


  // appending *.mtl file info
  obj_data.append("mtllib ../../../../" + wavefront_obj::mtl_filename + "\n");

  std::size_t last_vert_ind = 1;
  std::size_t last_norm_ind = 1;
  for(const auto &c3d_model : c3d_models)
  {
    std::size_t vert_num = c3d_model.second.verts.size();
    std::size_t norm_num = c3d_model.second.vertNorms.size();
    std::size_t poly_num = c3d_model.second.faces.size();
    std::size_t expected_vertices_per_poly =
      c3d_model.second.faces[0].verts.size();

    obj_data.append("g ");
    obj_data.append(c3d_model.first);
    obj_data.push_back('\n');

    // writing vertices
    for(std::size_t cur_vertex = 0; cur_vertex < vert_num; ++cur_vertex)
    {
      obj_data.push_back('v');
      for(std::size_t cur_coord = 0; cur_coord < volInt::axes_num; ++cur_coord)
      {
        obj_data.push_back(' ');
        to_string_precision<double>(
          c3d_model.second.verts[cur_vertex][cur_coord],
          float_precision_objs_string,
          obj_data);
      }
      obj_data.push_back('\n');
    }


    // writing normals
    for(std::size_t cur_normal = 0; cur_normal < norm_num; ++cur_normal)
    {
      obj_data.append("vn");
      for(std::size_t cur_coord = 0; cur_coord < volInt::axes_num; ++cur_coord)
      {
        obj_data.push_back(' ');
        to_string_precision<double>(
          c3d_model.second.vertNorms[cur_normal][cur_coord],
          float_precision_objs_string,
          obj_data);
      }
      obj_data.push_back('\n');
    }


    // writing polygons
    // color_id of any polygon should not be equal to max_colors_ids.
    unsigned int previous_color_id = c3d::color::string_to_id::max_colors_ids;
    int previous_wheel_weapon_id = -1;
    for(std::size_t cur_poly_num = 0; cur_poly_num < poly_num; ++cur_poly_num)
    {

//    if(c3d_model.second.faces[cur_poly_num].color_id !=
//         c3d::color::string_to_id::body &&
//       c3d_model.second.faces[cur_poly_num].color_shift)
//    {
//      throw std::runtime_error(
//        input_file_name_error + " file " + input_file_path.string() +
//        ". polygon " + std::to_string(cur_poly_num) +
//        " have non-body color_id " +
//        std::to_string(c3d_model.second.faces[cur_poly_num].color_id) +
//        " and non-zero color_shift " +
//        std::to_string(c3d_model.second.faces[cur_poly_num].color_shift) +
//        ". It is assumed that only polygons with body color_id " +
//        "can have non-zero color_shift.");
//    }


      // Only polygons with color_id "wheel", "weapon" or "attachement_point"
      // can have wheel_weapon_id not equal to -1.
      int cur_poly_wheel_weapon_id =
        c3d_model.second.faces[cur_poly_num].wheel_weapon_id;

      if(cur_poly_wheel_weapon_id < 0 ||
         std::find(c3d::color::wheel_weapon_color_ids.begin(),
                   c3d::color::wheel_weapon_color_ids.end(),
                   c3d_model.second.faces[cur_poly_num].color_id) ==
           c3d::color::wheel_weapon_color_ids.end())
      {
        cur_poly_wheel_weapon_id = -1;
      }

      if(c3d_model.second.faces[cur_poly_num].color_id != previous_color_id ||
         cur_poly_wheel_weapon_id != previous_wheel_weapon_id)
      {
        if(c3d_model.second.faces[cur_poly_num].color_id ==
           c3d::color::string_to_id::body)
        {
          obj_data.append(
            "usemtl body_offset_" +
            std::to_string(c3d_model.second.bodyColorOffset) +
            "_shift_" + std::to_string(c3d_model.second.bodyColorShift) +
            '\n');
        }
        else if(c3d_model.second.faces[cur_poly_num].color_id ==
                c3d::color::string_to_id::wheel)
        {
          obj_data.append("usemtl wheel");
          if(cur_poly_wheel_weapon_id >= 0)
          {
//          if(cur_poly_wheel_weapon_id < cur_wheel_data_size &&
//            cur_wheel_data[cur_poly_wheel_weapon_id].steer)
            if(c3d_model.second.wheels_steer.count(cur_poly_wheel_weapon_id))
            {
              obj_data.append("_steer");
            }
            if(c3d_model.second.wheels_ghost.count(cur_poly_wheel_weapon_id))
            {
              obj_data.append("_ghost");
            }
            obj_data.append(
              "_" + std::to_string(cur_poly_wheel_weapon_id + 1));
          }
          obj_data.push_back('\n');
        }
        else if(c3d_model.second.faces[cur_poly_num].color_id ==
                c3d::color::string_to_id::weapon)
        {
          obj_data.append("usemtl weapon");
          if(cur_poly_wheel_weapon_id >= 0)
          {
            obj_data.append(
              "_" + std::to_string(cur_poly_wheel_weapon_id + 1));
          }
          obj_data.push_back('\n');
        }
        else if(c3d_model.second.faces[cur_poly_num].color_id ==
                c3d::color::string_to_id::attachment_point)
        {
          obj_data.append("usemtl attachment_point");
          if(cur_poly_wheel_weapon_id >= 0)
          {
            obj_data.append(
              "_" + std::to_string(cur_poly_wheel_weapon_id + 1));
          }
          obj_data.push_back('\n');
        }
        else
        {
          obj_data.append(
            "usemtl " +
            c3d::color::ids.by<c3d::color::id>().at(
              c3d_model.second.faces[cur_poly_num].color_id) +
            '\n');
        }
        previous_color_id = c3d_model.second.faces[cur_poly_num].color_id;
        previous_wheel_weapon_id = cur_poly_wheel_weapon_id;
      }

      obj_data.push_back('f');
      for(std::size_t cur_poly_vert_num = 0;
          cur_poly_vert_num < expected_vertices_per_poly;
          ++cur_poly_vert_num)
      {
        obj_data.push_back(' ');
        to_string_precision<int>(
          c3d_model.second.faces[cur_poly_num].verts[cur_poly_vert_num] +
            last_vert_ind,
          "%i",
          obj_data);
        obj_data.append("//");
        to_string_precision<int>(
          c3d_model.second.faces[cur_poly_num].vertNorms[cur_poly_vert_num] +
            last_norm_ind,
          "%i",
          obj_data);
      }
      obj_data.push_back('\n');
    }

    last_vert_ind += vert_num;
    last_norm_ind += norm_num;
  }


  save_file(output_path,
            obj_data,
            file_flag::none,
            output_file_name_error);
}



} // namespace helpers
} // namespace tractor_converter
