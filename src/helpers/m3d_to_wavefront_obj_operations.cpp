#include "m3d_to_wavefront_obj_operations.hpp"



namespace tractor_converter{
namespace helpers{



m3d_to_wavefront_obj_model::m3d_to_wavefront_obj_model(
  const boost::filesystem::path &input_file_path_arg,
  const boost::filesystem::path &output_dir_path_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const volInt::polyhedron *example_weapon_model_arg,
  const volInt::polyhedron *weapon_attachment_point_arg,
  const volInt::polyhedron *ghost_wheel_model_arg,
  const volInt::polyhedron *center_of_mass_model_arg,
  double scale_size_arg,
  unsigned int float_precision_objs_arg,
  bitflag<m3d_to_obj_flag> flags_arg)
: vangers_model(
    input_file_path_arg,
    output_dir_path_arg / input_file_path_arg.stem(),
    input_file_name_error_arg,
    output_file_name_error_arg,
    example_weapon_model_arg,
    weapon_attachment_point_arg,
    ghost_wheel_model_arg,
    center_of_mass_model_arg),
  float_precision_objs(float_precision_objs_arg),
  flags(flags_arg)
{
  model_name = input_file_path.stem().string();

  scale_size = scale_size_arg;

  m3d_data =
    read_file(input_file_path,
              file_flag::binary | file_flag::read_all,
              0,
              0,
              read_all_dummy_size,
              input_file_name_error);
  m3d_data_cur_pos = 0;

  float_precision_objs_string =
    "%." + std::to_string(float_precision_objs) + "f";

  // 2 newline + 1 "v" +
  // 3 "num of coords" * (1 space + 3 digits left to dot + 1 dot +
  //   float_precision_objs digits right to dot ) +
  // 5
  expected_medium_vertex_size =
    2 + 1 + 3 * (1 + 3 + 1 + float_precision_objs) + 5;

  // +1 because "vn" instead of "v"
  expected_medium_normal_size = 1 + expected_medium_vertex_size;

  non_steer_ghost_wheels_num = 0;
}



void m3d_to_wavefront_obj_model::mechos_m3d_to_wavefront_objs()
{
  boost::filesystem::create_directory(output_dir_path);

  volInt::polyhedron main_model = read_c3d(c3d::c3d_type::regular);
  // TEST
//  std::cout << "-----------------------------" << '\n';
//  std::cout << "Volume of main of " <<
//    input_file_path.string() << '\n';
//  std::cout << main_model.check_volume() << '\n';

  // IMPORTANT! Header data must be acquired before writing c3d to obj.
  read_m3d_header_data();

  std::vector<volInt::polyhedron> steer_wheels_models;
  std::vector<volInt::polyhedron> non_steer_ghost_wheels_models;
  if(n_wheels)
  {
    steer_wheels_models = read_m3d_wheels_data();
    mark_wheels(main_model, steer_wheels_models);
    if(ghost_wheel_model)
    {
      get_ghost_wheels(main_model,
                       steer_wheels_models,
                       non_steer_ghost_wheels_models);
    }
  }
  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    debris_models;
  std::vector<volInt::polyhedron> debris_bound_models;
  if(n_debris)
  {
    read_m3d_debris_data(debris_models, debris_bound_models);
    move_debris_to_offset(debris_models, debris_bound_models);
    save_m3d_debris_data(debris_models, debris_bound_models);
  }
  c3d_to_wavefront_obj("_main_bound", c3d::c3d_type::bound);

  weapon_slots_existence = read_var_from_m3d<int, int>();
  if(weapon_slots_existence)
  {
    read_m3d_weapon_slots();
  }


  if(n_wheels || weapon_slots_existence)
  {
    merge_helper_reserve_space_in_main(main_model,
                                       steer_wheels_models,
                                       non_steer_ghost_wheels_models);
  }
  if(n_wheels)
  {
//  std::cout << "\n\ninput_file_path.string(): " <<
//    input_file_path.string() << "\n\n";
//  std::vector<volInt::polyhedron> steer_wheels_models =
//    read_m3d_wheels_data();
    merge_main_model_with_wheels(main_model,
                                 steer_wheels_models,
                                 non_steer_ghost_wheels_models);
  }
//if(center_of_mass_model)
//{
//  merge_model_with_center_of_mass(main_model);
//}



  // 1 for main model
  std::size_t main_models_to_reserve = 1;
  if(weapon_slots_existence && example_weapon_model)
  {
    main_models_to_reserve += m3d::weapon_slot::max_slots;
  }
  if(center_of_mass_model)
  {
    // 1 for center of mass model
    main_models_to_reserve += 1;
  }

  std::unordered_map<std::string, volInt::polyhedron> main_models;
  main_models.reserve(main_models_to_reserve);
  main_models[wavefront_obj::main_obj_name] = std::move(main_model);

  if(weapon_slots_existence && example_weapon_model)
  {
    add_weapons_to_models_map(main_models);
  }
  if(center_of_mass_model)
  {
    add_center_of_mass_to_models_map(
      main_models,
      main_models[wavefront_obj::main_obj_name].rcm);
  }



  save_c3d_as_wavefront_obj(main_models, "_main");

  save_file_cfg_m3d(main_models[wavefront_obj::main_obj_name], &debris_models);
}



volInt::polyhedron m3d_to_wavefront_obj_model::weapon_m3d_to_wavefront_objs()
{
  boost::filesystem::create_directory(output_dir_path);

  volInt::polyhedron main_model = read_c3d(c3d::c3d_type::regular);
  // TEST
//  std::cout << "-----------------------------" << '\n';
//  std::cout << "Volume of main of " <<
//    input_file_path.string() << '\n';
//  std::cout << main_model.check_volume() << '\n';

  // IMPORTANT! Header data must be acquired before writing c3d to obj.
  read_m3d_header_data();



  // 1 for main model
  std::size_t main_models_to_reserve = 1;
  if(weapon_attachment_point)
  {
    // 1 for attachment point model
    main_models_to_reserve += 1;
  }
  if(center_of_mass_model)
  {
    // 1 for center of mass model
    main_models_to_reserve += 1;
  }

  std::unordered_map<std::string, volInt::polyhedron> main_models;
  main_models.reserve(main_models_to_reserve);
  main_models[wavefront_obj::main_obj_name] = main_model;



  if(weapon_attachment_point)
  {
    add_attachment_point_to_models_map(
      main_models,
      main_models[wavefront_obj::main_obj_name].offset_point());
  }
  if(center_of_mass_model)
  {
    add_center_of_mass_to_models_map(
      main_models,
      main_models[wavefront_obj::main_obj_name].rcm);
  }
  save_c3d_as_wavefront_obj(main_models, "_main");

  if(n_wheels)
  {
    throw std::runtime_error(
      input_file_name_error + " file " +
      input_file_path.string() +
      ". Non-mechos model have non-zero n_wheels value.");
  }
  // TEST
  /*
  std::cout << "weapon_slots_existence: " << weapon_slots_existence << '\n';
  for(std::size_t cur_slot = 0;
      cur_slot < m3d::weapon_slot::max_slots;
      ++cur_slot)
  {
    std::cout << "cur_slot: " << cur_slot << '\n';
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      std::cout << "R_slot[" << cur_coord << "]" <<
        cur_weapon_slot_data[cur_slot].R_slot[cur_coord] << '\n';
    }
    std::cout << "slot_data_el.location_angle_of_slot: " <<
      cur_weapon_slot_data[cur_slot].location_angle_of_slot << '\n';
  }
  */
  c3d_to_wavefront_obj("_main_bound", c3d::c3d_type::bound);

  weapon_slots_existence = read_var_from_m3d<int, int>();
  if(weapon_slots_existence)
  {
    throw std::runtime_error(
      input_file_name_error + " file " +
      input_file_path.string() +
      ". Non-mechos model have non-zero weapon_slots_existence.");
  }

  save_file_cfg_m3d(main_models[wavefront_obj::main_obj_name]);


  if(weapon_attachment_point)
  {
    merge_model_with_weapon_attachment_point(main_model);
  }

  return main_model;
}



void m3d_to_wavefront_obj_model::animated_a3d_to_wavefront_objs()
{
  boost::filesystem::create_directory(output_dir_path);

  // IMPORTANT! Header data must be acquired before writing c3d to obj.
  read_a3d_header_data();

  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    models(n_models);

  // 1 for main model
  std::size_t animated_models_to_reserve = 1;
  if(center_of_mass_model)
  {
    // 1 for center of mass model
    animated_models_to_reserve += 1;
  }

  for(std::size_t cur_animated = 0; cur_animated < n_models; ++cur_animated)
  {
    models[cur_animated].reserve(animated_models_to_reserve);
    models[cur_animated][wavefront_obj::main_obj_name] =
      read_c3d(c3d::c3d_type::regular);

    // TEST
//    std::cout << "-----------------------------" << '\n';
//    std::cout << "Volume of animated " << cur_animated <<
//      " of " << input_file_path.string() << '\n';
//    std::cout <<
//      models.at(cur_animated).
//        at(wavefront_obj::main_obj_name).check_volume() << '\n';

    if(center_of_mass_model)
    {
//    merge_model_with_center_of_mass(models[cur_animated]);
      add_center_of_mass_to_models_map(
        models[cur_animated],
        models[cur_animated][wavefront_obj::main_obj_name].rcm);
    }
    save_c3d_as_wavefront_obj(
      models[cur_animated],
      "_" + std::to_string(cur_animated + 1));
  }
  save_file_cfg_a3d(models);
}



void m3d_to_wavefront_obj_model::other_m3d_to_wavefront_objs()
{
  boost::filesystem::create_directory(output_dir_path);



  volInt::polyhedron main_model = read_c3d(c3d::c3d_type::regular);
  // TEST
//  std::cout << "-----------------------------" << '\n';
//  std::cout << "Volume of main of " <<
//    input_file_path.string() << '\n';
//  std::cout << main_model.check_volume() << '\n';

  // IMPORTANT! Header data must be acquired before writing c3d to obj.
  read_m3d_header_data();



  // 1 for main model
  std::size_t main_models_to_reserve = 1;
  if(center_of_mass_model)
  {
    // 1 for center of mass model
    main_models_to_reserve += 1;
  }

  std::unordered_map<std::string, volInt::polyhedron> main_models;
  main_models.reserve(main_models_to_reserve);

  main_models[wavefront_obj::main_obj_name] = std::move(main_model);

  if(center_of_mass_model)
  {
//  merge_model_with_center_of_mass(main_model);
    add_center_of_mass_to_models_map(
      main_models, main_models[wavefront_obj::main_obj_name].rcm);
  }

  save_c3d_as_wavefront_obj(main_models, "_main");

  if(n_wheels)
  {
    throw std::runtime_error(
      input_file_name_error + " file " +
      input_file_path.string() +
      ". Non-mechos model have non-zero n_wheels value.");
  }

  c3d_to_wavefront_obj("_main_bound", c3d::c3d_type::bound);

  weapon_slots_existence = read_var_from_m3d<int, int>();
  if(weapon_slots_existence)
  {
    throw std::runtime_error(
      input_file_name_error + " file " +
      input_file_path.string() +
      ". Non-mechos model have non-zero weapon_slots_existence.");
  }

  save_file_cfg_m3d(main_models[wavefront_obj::main_obj_name]);
}





std::vector<double> m3d_to_wavefront_obj_model::read_vertex()
{
  std::vector<double> vert =
    read_vec_var_from_m3d_scaled<float, double>(3);
  std::vector<double> discarded_less_preciese_vert =
    read_vec_var_from_m3d_scaled<char, double>(3);
  int discarded_sort_info = read_var_from_m3d<int, int>();
  return vert;
}

void m3d_to_wavefront_obj_model::read_vertices(volInt::polyhedron &model)
{
  for(int cur_vertex = 0; cur_vertex < model.numVerts; ++cur_vertex)
  {
    model.verts[cur_vertex] = read_vertex();
  }
}



std::vector<double> m3d_to_wavefront_obj_model::read_normal(
  bitflag<normal_flag> flags = normal_flag::sort_info)
{
  std::vector<double> norm =
    volInt::vector_scale(wavefront_obj::vector_scale_val,
                         read_vec_var_from_m3d<char, double>(3));
  unsigned char discarded_n_power =
    read_var_from_m3d<unsigned char, unsigned char>();
  if(flags & normal_flag::sort_info)
  {
    int discarded_sort_info = read_var_from_m3d<int, int>();
  }
  return norm;
}

void m3d_to_wavefront_obj_model::read_normals(volInt::polyhedron &model)
{
  for(int cur_normal = 0; cur_normal < model.numVertNorms; ++cur_normal)
  {
    model.vertNorms[cur_normal] = read_normal();
  }
}


volInt::face m3d_to_wavefront_obj_model::read_polygon(
  const volInt::polyhedron &model, std::size_t cur_poly)
{
  int numVerts = read_var_from_m3d<int, int>();
  if(model.numVertsPerPoly != numVerts)
  {
    throw std::runtime_error(
      "In " + input_file_name_error +
      " file " + input_file_path.string() +
      " polygon " + std::to_string(cur_poly) +
      " at position " +
      std::to_string(m3d_data_cur_pos - sizeof(int)) +
      " have unexpected number of vertices " +
      std::to_string(numVerts) +
      ". Expected " + std::to_string(model.numVertsPerPoly) + ".");
  }
  volInt::face poly(numVerts);

  int discarded_sort_info = read_var_from_m3d<int, int>();

  poly.color_id = read_var_from_m3d<unsigned int, unsigned int>();
  if(poly.color_id >= c3d::color::string_to_id::max_colors_ids)
  {
    poly.color_id = c3d::color::string_to_id::body;
  }

  unsigned int color_shift = read_var_from_m3d<unsigned int, unsigned int>();
  if(color_shift)
  {
    std::cout << "\n\n" <<
      input_file_name_error << " file " <<
      input_file_path.string() <<
      ". polygon " << std::to_string(cur_poly) <<
      " at position " +
      std::to_string(m3d_data_cur_pos - sizeof(unsigned int)) +
      " have non-zero color_shift " << std::to_string(color_shift) <<
      ". It is assumed that color_shift of any polygon is always 0." << '\n';
  }


  std::vector<double> discarded_flat_normal =
    read_normal(normal_flag::none);
  std::vector<double> discarded_medium_vert =
    read_vec_var_from_m3d_scaled<char, double>(3);

  // Note the reverse order of vertices.
  for(int cur_poly_vertex = numVerts - 1;
      cur_poly_vertex != -1;
      --cur_poly_vertex)
  {
    poly.verts[cur_poly_vertex] = read_var_from_m3d<int, int>();
    poly.vertNorms[cur_poly_vertex] = read_var_from_m3d<int, int>();
  }

  return poly;
}

void m3d_to_wavefront_obj_model::read_polygons(volInt::polyhedron &model)
{
  for(int cur_poly = 0; cur_poly < model.numFaces; ++cur_poly)
  {
    model.faces[cur_poly] = read_polygon(model, cur_poly);
  }
}


void m3d_to_wavefront_obj_model::read_sorted_polygon_indices(
  volInt::polyhedron &model)
{
    m3d_data_cur_pos += model.numFaces * c3d::polygon_sort_info::size;
}



volInt::polyhedron m3d_to_wavefront_obj_model::read_c3d(
  c3d::c3d_type cur_c3d_type)
{
  int expected_vertices_per_poly;
  if(cur_c3d_type == c3d::c3d_type::regular)
  {
    expected_vertices_per_poly = c3d::regular_model_vertices_per_polygon;
  }
  else
  {
    expected_vertices_per_poly = c3d::bound_model_vertices_per_polygon;
  }

  int version = read_var_from_m3d<int, int>();

  if(version != c3d::version_req)
  {
    throw std::runtime_error(
      input_file_name_error + " file " +
      input_file_path.string() + " have unexpected c3d_version " +
      std::to_string(version) + " at position " +
      std::to_string(m3d_data_cur_pos - sizeof(int)) +
      ". Expected " + std::to_string(c3d::version_req) + ".");
  }

  int numVerts = read_var_from_m3d<int, int>();
  int numVertNorms = read_var_from_m3d<int, int>();
  int numFaces = read_var_from_m3d<int, int>();
  int discarded_numVertTotal = read_var_from_m3d<int, int>();

  volInt::model_extreme_points discarded_extreme_points;
  discarded_extreme_points.max() =
    read_vec_var_from_m3d_scaled<int, double>(3);
  discarded_extreme_points.min() =
    read_vec_var_from_m3d_scaled<int, double>(3);

  volInt::model_offset offset_point(
    read_vec_var_from_m3d_scaled<int, double>(3));

  double discarded_rmax = read_var_from_m3d_scaled<int, double>();

  std::vector<int> discarded_phi_psi_tetta =
    read_vec_var_from_m3d<int, int>(3);

  double volume = read_var_from_m3d_scaled<double, double>(3.0);
  std::vector<double> rcm = read_vec_var_from_m3d_scaled<double, double>(3);
  std::vector<std::vector<double>> J =
    read_nest_vec_var_from_m3d_scaled<double, double>(3, 3, 5.0);



  volInt::polyhedron cur_model(numVerts,
                               numVertNorms,
                               numFaces,
                               expected_vertices_per_poly);
  cur_model.offset = offset_point;
  cur_model.volume = volume;
  cur_model.rcm = rcm;
  cur_model.J = J;



  read_vertices(cur_model);
  read_normals(cur_model);
  read_polygons(cur_model);

  read_sorted_polygon_indices(cur_model);

  cur_model.faces_calc_params_inv_neg_vol();

  return cur_model;
}



void m3d_to_wavefront_obj_model::save_c3d_as_wavefront_obj(
  std::unordered_map<std::string, volInt::polyhedron> &c3d_models,
  const std::string &output_file_prefix)
{
  boost::filesystem::path file_to_save = output_dir_path;
  file_to_save.append(model_name + output_file_prefix + ".obj",
                      boost::filesystem::path::codecvt());

  c3d_models[wavefront_obj::main_obj_name].bodyColorOffset = body_color_offset;
  c3d_models[wavefront_obj::main_obj_name].bodyColorShift = body_color_shift;

  c3d_models[wavefront_obj::main_obj_name].wavefront_obj_path =
    file_to_save.string();

  save_volInt_as_wavefront_obj(c3d_models,
                               file_to_save,
                               output_file_name_error,
                               float_precision_objs_string,
                               expected_medium_vertex_size,
                               expected_medium_normal_size);
}

void m3d_to_wavefront_obj_model::save_c3d_as_wavefront_obj(
  volInt::polyhedron &c3d_model,
  const std::string &output_file_prefix)
{
    // TEST
//  std::cout << "-----------------------------" << '\n';
//  std::cout << "Volume of " << output_file_prefix << " of " <<
//    input_file_path.string() << '\n';
//  std::cout << c3d_model.check_volume() << '\n';

  std::unordered_map<std::string, volInt::polyhedron> c3d_models
    {{wavefront_obj::main_obj_name, c3d_model}};
  save_c3d_as_wavefront_obj(c3d_models, output_file_prefix);
}



void m3d_to_wavefront_obj_model::c3d_to_wavefront_obj(
  const std::string &output_file_prefix,
  c3d::c3d_type cur_c3d_type)
{
  volInt::polyhedron c3d_model = read_c3d(cur_c3d_type);
  save_c3d_as_wavefront_obj(c3d_model, output_file_prefix);
}



void m3d_to_wavefront_obj_model::read_m3d_header_data()
{
  std::vector<double> discarded_max_point =
    read_vec_var_from_m3d_scaled<int, double>(3);
  double discarded_rmax = read_var_from_m3d_scaled<int, double>();

  n_wheels = read_var_from_m3d<int, int>();
  n_debris = read_var_from_m3d<int, int>();
  body_color_offset = read_var_from_m3d<int, int>();
  body_color_shift = read_var_from_m3d<int, int>();
}



void m3d_to_wavefront_obj_model::read_a3d_header_data()
{
  n_models = read_var_from_m3d<int, int>();

  std::vector<double> discarded_max_point =
    read_vec_var_from_m3d_scaled<int, double>(3);
  double discarded_rmax =
    read_var_from_m3d_scaled<int, double>();

  body_color_offset = read_var_from_m3d<int, int>();
  body_color_shift = read_var_from_m3d<int, int>();
}



void m3d_to_wavefront_obj_model::read_m3d_wheel_data(
  std::vector<volInt::polyhedron> &wheel_models, std::size_t wheel_id)
{
  cur_wheel_data[wheel_id].steer = read_var_from_m3d<int, int>();
  cur_wheel_data[wheel_id].r =
    read_vec_var_from_m3d_scaled<double, double>(3);
  cur_wheel_data[wheel_id].width =
    read_var_from_m3d_scaled<int, double>();
  cur_wheel_data[wheel_id].radius =
    read_var_from_m3d_scaled<int, double>();
  int discarded_bound_index = read_var_from_m3d<int, int>();

  if(cur_wheel_data[wheel_id].steer)
  {
    wheel_models.push_back(read_c3d(c3d::c3d_type::regular));
    cur_wheel_data[wheel_id].wheel_model_index = wheel_models.size() - 1;
  }
}

std::vector<volInt::polyhedron>
  m3d_to_wavefront_obj_model::read_m3d_wheels_data()
{
  std::vector<volInt::polyhedron> wheel_models;
  wheel_models.reserve(n_wheels);

  cur_wheel_data = std::vector<wheel_data>(n_wheels);
  for(int cur_wheel_num = 0; cur_wheel_num < n_wheels; ++cur_wheel_num)
  {
    read_m3d_wheel_data(wheel_models, cur_wheel_num);
  }
  return wheel_models;
}





void m3d_to_wavefront_obj_model::mark_wheels_helper_get_wheels(
  const std::vector<const volInt::face*> &polygons,
  volInt::polyhedron &main_model,
  std::vector<std::vector<const volInt::face*>> &end_wheels_groups,
  std::vector<std::vector<double>> &wheels_centers) const
{
  end_wheels_groups =
    volInt::get_groups_of_connected_items<const volInt::face*>(
      polygons,
      [](const volInt::face* first, const volInt::face* second)->bool
        {
          // Checking if cur_poly and poly_to_compare have same vertex.
          for(std::size_t cur_vertex_num = 0;
              cur_vertex_num < c3d::regular_model_vertices_per_polygon;
              ++cur_vertex_num)
          {
            if(std::find(first->verts.begin(),
                         first->verts.end(),
                         second->verts[cur_vertex_num]) !=
               first->verts.end())
            {
              return true;
            }
          }
          return false;
        });

  std::size_t end_wheels_groups_size = end_wheels_groups.size();
  wheels_centers.reserve(end_wheels_groups_size);
  for(std::size_t cur_group = 0;
      cur_group < end_wheels_groups_size;
      ++cur_group)
  {
    wheels_centers.push_back(
      main_model.get_model_center(end_wheels_groups[cur_group]));
  }
}



void
  m3d_to_wavefront_obj_model::mark_helper_move_non_steering_wheels_to_center(
    volInt::polyhedron &model) const
{
  for(const auto wheel_non_ghost_num : model.wheels_non_ghost)
  {
    std::vector<const std::vector<double>*> wheel_vertices =
      model.get_vertices_by_ids(c3d::color::string_to_id::wheel,
                                wheel_non_ghost_num);
    const std::vector<double> wheel_center =
      model.get_model_center(wheel_vertices);

    model.move_model_to_point(
      wheel_vertices,
      volInt::vector_minus(cur_wheel_data[wheel_non_ghost_num].r,
                           wheel_center));
  }
}



// Marking non-steering wheels and figuring which wheels are ghost ones.
void m3d_to_wavefront_obj_model::mark_wheels(
  volInt::polyhedron &main_model,
  const std::vector<volInt::polyhedron> &steer_wheels_models)
{
  for(std::size_t i = 0, cur_wheel_data_size = cur_wheel_data.size();
      i < cur_wheel_data_size;
      ++i)
  {
    if(cur_wheel_data[i].steer)
    {
      main_model.wheels_steer.insert(i);
    }
    else
    {
      main_model.wheels_non_steer.insert(i);
    }
  }

  // Creating groups of non-steering wheels polygons.
  std::vector<const volInt::face*> non_steering_wheels_polygons =
    main_model.get_polygons_by_color(c3d::color::string_to_id::wheel);


  std::vector<std::vector<const volInt::face*>> wheels_groups;
  std::vector<std::vector<double>> wheels_centers;

  mark_wheels_helper_get_wheels(non_steering_wheels_polygons,
                                main_model,
                                wheels_groups,
                                wheels_centers);


//unsigned int non_steering_wheels_num = 0;
//for(const auto &cur_wheel_data_el : cur_wheel_data)
//{
//  if(!cur_wheel_data[cur_wheel_data_el].steer)
//  {
//    ++non_steering_wheels_num;
//  }
//}

  // Trying to figure out which part of model is current non-steering wheel.
  // Polygons which belong to this wheel are getting their "wheel_weapon_id"
  // changed to ease material name generation.

//for(int cur_wheel_num = 0, cur_wheel_data_size = cur_wheel_data.size();
//    cur_wheel_num < cur_wheel_data_size;
//    ++cur_wheel_num)
  for(std::size_t model_wheel_center_num = 0,
        wheels_centers_size = wheels_centers.size();
      model_wheel_center_num < wheels_centers_size;
      ++model_wheel_center_num)
  {
    std::size_t closest_wheel_data_num = 0;
    double closest_distance = std::numeric_limits<double>::max();
//  for(std::size_t model_wheel_center_num = 0,
//        wheels_centers_size = wheels_centers.size();
//      model_wheel_center_num < wheels_centers_size;
//      ++model_wheel_center_num)
    for(int cur_wheel_num = 0, cur_wheel_data_size = cur_wheel_data.size();
        cur_wheel_num < cur_wheel_data_size;
        ++cur_wheel_num)
    {
      if(!cur_wheel_data[cur_wheel_num].steer)
      {
        double distance =
          volInt::vector_length_between(wheels_centers[model_wheel_center_num],
                                        cur_wheel_data[cur_wheel_num].r);
        if(distance < closest_distance)
        {
          closest_wheel_data_num = cur_wheel_num;
          closest_distance = distance;
        }
      }
    }
    // TEST
//  std::cout << "cur_wheel_data[cur_wheel_num].r: " <<
//    cur_wheel_data[cur_wheel_num].r[0] << "; " <<
//    cur_wheel_data[cur_wheel_num].r[1] << "; " <<
//    cur_wheel_data[cur_wheel_num].r[2] << '\n';
//  std::cout << "wheels_centers[closest_model_wheel_num]: " <<
//    wheels_centers[closest_model_wheel_num][0] << "; " <<
//    wheels_centers[closest_model_wheel_num][1] << "; " <<
//    wheels_centers[closest_model_wheel_num][2] << '\n';
//  std::cout << "closest_distance: " << std::sqrt(closest_distance) << '\n';
    for(auto cur_poly : wheels_groups[model_wheel_center_num])
    {
      // Assuming that volInt::face* points to polygon of non-const main_model.
      const_cast<volInt::face*>(cur_poly)->wheel_weapon_id =
        closest_wheel_data_num;
    }

    cur_wheel_data[closest_wheel_data_num].ghost = 0;
    // Note that insert() won't create duplicates
    // since wheels_non_ghost is std::unordered_set.
    main_model.wheels_non_ghost.insert(closest_wheel_data_num);
  }

  mark_helper_move_non_steering_wheels_to_center(main_model);

  // Needed to properly calculate volume
  // right after moving non-steering wheels.
  // Not really used but useful for debugging.
  main_model.faces_calc_params();



  // Marking ghost wheels.
  non_steer_ghost_wheels_num = 0;
  for(std::size_t i = 0, cur_wheel_data_size = cur_wheel_data.size();
      i < cur_wheel_data_size;
      ++i)
  {
    // If there is no group found for wheel center it's a ghost wheel.
    if(cur_wheel_data[i].steer)
    {
      if(steer_wheels_models.
           at(cur_wheel_data[i].wheel_model_index).numFaces ==
         0)
      {
        cur_wheel_data[i].ghost = 1;
        main_model.wheels_ghost.insert(i);
      }
      else
      {
        cur_wheel_data[i].ghost = 0;
        main_model.wheels_non_ghost.insert(i);
      }
    }
    else
    {
      if(!main_model.wheels_non_ghost.count(i))
      {
        cur_wheel_data[i].ghost = 1;
        main_model.wheels_ghost.insert(i);
        ++non_steer_ghost_wheels_num;
      }
    }
  }
}



// Copy ghost wheel model and scale it to specified width and radius.
volInt::polyhedron
  m3d_to_wavefront_obj_model::get_ghost_wheels_helper_generate_wheel(
    double width,
    double radius,
    int wheel_id)
{
  volInt::polyhedron cur_ghost_wheel = *ghost_wheel_model;

  cur_ghost_wheel.get_extreme_points();
  double given_width = cur_ghost_wheel.xmax() - cur_ghost_wheel.xmin();
  double width_multiplier = cur_wheel_data[wheel_id].width / given_width;
  double given_radius =
    (cur_ghost_wheel.zmax() - cur_ghost_wheel.zmin()) / 2;
  double radius_multiplier = cur_wheel_data[wheel_id].radius / given_radius;

  // TEST
  /*
  std::cout << "cur_wheel_data[" << wheel_id << "].width: " <<
    cur_wheel_data[wheel_id].width << '\n';
  std::cout << "given_width: " <<
    given_width << '\n';
  std::cout << "width_multiplier: " <<
    width_multiplier << '\n';

  std::cout << "cur_wheel_data[" << wheel_id << "].radius: " <<
    cur_wheel_data[wheel_id].radius << '\n';
  std::cout << "given_radius: " <<
    given_width << '\n';
  std::cout << "radius_multiplier: " <<
    radius_multiplier << '\n';
  */

  std::vector<double> multiplier =
    {
      width_multiplier,
      radius_multiplier,
      radius_multiplier,
    };

  for(auto &&cur_vert : cur_ghost_wheel.verts)
  {
    volInt::vector_multiply_self(cur_vert, multiplier);
  }

// TEST
// Both variables are set at merge_helper_move_model_into_main() function.
//  for(auto &&cur_poly : cur_ghost_wheel.faces)
//  {
//    cur_poly.color_id = c3d::color::string_to_id::weapon;
//    cur_poly.wheel_weapon_id = wheel_id;
//  }

  return cur_ghost_wheel;
}



void m3d_to_wavefront_obj_model::get_ghost_wheels(
  const volInt::polyhedron &main_model,
  std::vector<volInt::polyhedron> &steer_wheels_models,
  std::vector<volInt::polyhedron> &non_steer_ghost_wheels_models)
{
  non_steer_ghost_wheels_models.reserve(non_steer_ghost_wheels_num);

  for(std::size_t i = 0, cur_wheel_data_size = cur_wheel_data.size();
      i < cur_wheel_data_size;
      ++i)
  {
    if(cur_wheel_data[i].steer)
    {
      if(cur_wheel_data[i].ghost)
      {
        steer_wheels_models[cur_wheel_data[i].wheel_model_index] =
          get_ghost_wheels_helper_generate_wheel(
            cur_wheel_data[i].width,
            cur_wheel_data[i].radius,
            i);
      }
    }
    else
    {
      if(cur_wheel_data[i].ghost)
      {
        non_steer_ghost_wheels_models.push_back(
          get_ghost_wheels_helper_generate_wheel(
            cur_wheel_data[i].width,
            cur_wheel_data[i].radius,
            i));
        cur_wheel_data[i].wheel_model_index =
          non_steer_ghost_wheels_models.size() - 1;
      }
    }
  }
}



void m3d_to_wavefront_obj_model::merge_helper_reserve_space_in_main(
  volInt::polyhedron &main_model,
  const std::vector<volInt::polyhedron> &steer_wheels_models,
  const std::vector<volInt::polyhedron> &non_steer_ghost_wheels_models) const
{
  int new_main_model_num_verts = main_model.numVerts;
  int new_main_model_num_vert_norms = main_model.numVertNorms;
  int new_main_model_num_poly = main_model.numFaces;
  if(n_wheels)
  {
    for(const auto &model_to_merge : steer_wheels_models)
    {
      new_main_model_num_verts += model_to_merge.numVerts;
      new_main_model_num_vert_norms += model_to_merge.numVertNorms;
      new_main_model_num_poly += model_to_merge.numFaces;
    }
    for(const auto &model_to_merge : non_steer_ghost_wheels_models)
    {
      new_main_model_num_verts += model_to_merge.numVerts;
      new_main_model_num_vert_norms += model_to_merge.numVertNorms;
      new_main_model_num_poly += model_to_merge.numFaces;
    }
  }
  // Example weapon models are no longer merged with main model.
//if(weapon_slots_existence && example_weapon_model)
//{
//  new_main_model_num_verts +=
//    example_weapon_model->numVerts * m3d::weapon_slot::max_slots;
//  new_main_model_num_vert_norms +=
//    example_weapon_model->numVertNorms * m3d::weapon_slot::max_slots;
//  new_main_model_num_poly +=
//    example_weapon_model->numFaces * m3d::weapon_slot::max_slots;
//}
  // Center of mass model is no longer merged with main model.
//if(center_of_mass_model)
//{
//  new_main_model_num_verts += center_of_mass_model->numVerts;
//  new_main_model_num_vert_norms += center_of_mass_model->numVertNorms;
//  new_main_model_num_poly += center_of_mass_model->numFaces;
//}
  main_model.verts.reserve(new_main_model_num_verts);
  main_model.vertNorms.reserve(new_main_model_num_vert_norms);
  main_model.faces.reserve(new_main_model_num_poly);
}



void m3d_to_wavefront_obj_model::merge_helper_move_model_into_main(
  volInt::polyhedron &main_model,
  volInt::polyhedron &model_to_move,
  point new_position,
  double new_angle,
  int wheel_weapon_num,
  merge_model_type merge_type) const
{
  unsigned int merge_model_color_id;
  if(merge_type == merge_model_type::wheel)
  {
    // Changing model_to_move so calculated center of extreme coordinates
    // and actual center are the same.
    model_to_move.move_coord_system_to_center();
    merge_model_color_id = c3d::color::string_to_id::wheel;
  }
  else if(merge_type == merge_model_type::weapon)
  {
    // Changing position of weapon as it is changed in vangers source code.
    /*
    // VANGERS SOURCE
    Vector off =
      A_c2p *
      Vector(
        data_in_slots[i] -> model -> x_off,
        data_in_slots[i] -> model -> y_off,
        data_in_slots[i] -> model -> z_off);
    data_in_slots[i] -> model -> draw_child(R_slots[i] - off,A_c2p,A_p2c);
    */
    std::vector<double> weapon_offset = model_to_move.offset_point();
    volInt::rotate_point_by_axis(weapon_offset,
                                 new_angle,
                                 volInt::rotation_axis::y);
    volInt::vector_minus_self(new_position, weapon_offset);

    merge_model_color_id = c3d::color::string_to_id::weapon;
  }
  else if(merge_type == merge_model_type::attachment_point)
  {
    merge_model_color_id = c3d::color::string_to_id::attachment_point;
  }
  else if(merge_type == merge_model_type::center_of_mass)
  {
    merge_model_color_id = c3d::color::string_to_id::center_of_mass;
  }
  // Changing color_id for model.
  set_color_id(model_to_move, merge_model_color_id, wheel_weapon_num);



  // Changing model_to_move's y angle.
  model_to_move.rotate_by_axis(new_angle, volInt::rotation_axis::y);



  // Changing coordinates of all vertices of model_to_move
  // so it will be in the right place.
  // new_position is center coordinates of model_to_move
  // relative to main model center.
  model_to_move.move_model_to_point(new_position);

  // Since all vertices and norms are appended to main model
  // all vertices and norm indexes of moved polygons must be updated.
  for(auto &&cur_poly : model_to_move.faces)
  {
    for(auto &&cur_vert_index : cur_poly.verts)
    {
      cur_vert_index += main_model.numVerts;
    }
    for(auto &&cur_vert_norm_index : cur_poly.vertNorms)
    {
      cur_vert_norm_index += main_model.numVertNorms;
    }
  }

  // moving vertices, vertices' normals and polygons of wheel into main model
  std::move(model_to_move.verts.begin(),
            model_to_move.verts.end(),
            std::back_inserter(main_model.verts));
  std::move(model_to_move.vertNorms.begin(),
            model_to_move.vertNorms.end(),
            std::back_inserter(main_model.vertNorms));
  std::move(model_to_move.faces.begin(),
            model_to_move.faces.end(),
            std::back_inserter(main_model.faces));

  main_model.numVerts += model_to_move.numVerts;
  main_model.numVertNorms += model_to_move.numVertNorms;
  main_model.numFaces += model_to_move.numFaces;
}



// merging steering wheels into main model
void m3d_to_wavefront_obj_model::merge_main_model_with_wheels(
  volInt::polyhedron &main_model,
  std::vector<volInt::polyhedron> &steer_wheels_models,
  std::vector<volInt::polyhedron> &non_steer_ghost_wheels_models) const
{
  // inserting steering wheels into main model
  for(int cur_wheel_num = 0;
      cur_wheel_num < cur_wheel_data.size();
      ++cur_wheel_num)
  {
    if(cur_wheel_data[cur_wheel_num].steer)
    {
      merge_helper_move_model_into_main(
        main_model,
        steer_wheels_models[cur_wheel_data[cur_wheel_num].wheel_model_index],
        cur_wheel_data[cur_wheel_num].r,
        0.0,
        cur_wheel_num,
        merge_model_type::wheel);
    }
    else
    {
      if(ghost_wheel_model && cur_wheel_data[cur_wheel_num].ghost)
      {
        merge_helper_move_model_into_main(
          main_model,
          non_steer_ghost_wheels_models
            [cur_wheel_data[cur_wheel_num].wheel_model_index],
          cur_wheel_data[cur_wheel_num].r,
          0.0,
          cur_wheel_num,
          merge_model_type::wheel);
      }
    }
  }
}



// Not used.
void m3d_to_wavefront_obj_model::merge_main_model_with_weapons(
  volInt::polyhedron &main_model) const
{
  // inserting weapon models into main model
  for(int cur_weapon_num = 0;
      cur_weapon_num < m3d::weapon_slot::max_slots;
      ++cur_weapon_num)
  {
    if(flags & m3d_to_obj_flag::extract_nonexistent_weapons ||
       cur_weapon_slot_data[cur_weapon_num].exists)
    {
      volInt::polyhedron temp_weapon_model = *example_weapon_model;
      merge_helper_move_model_into_main(
        main_model,
        temp_weapon_model,
        cur_weapon_slot_data[cur_weapon_num].R_slot,
        cur_weapon_slot_data[cur_weapon_num].location_angle_of_slot,
        cur_weapon_num,
        merge_model_type::weapon);
    }
  }
}



// Not used.
void m3d_to_wavefront_obj_model::merge_model_with_center_of_mass(
  volInt::polyhedron &model) const
{
  volInt::polyhedron temp_center_of_mass_model = *center_of_mass_model;
  merge_helper_move_model_into_main(
    model,
    temp_center_of_mass_model,
    model.rcm,
    0.0,
    -1,
    merge_model_type::center_of_mass);
}



void m3d_to_wavefront_obj_model::merge_model_with_weapon_attachment_point(
  volInt::polyhedron &main_model) const
{
  std::vector<double> attachment_point = main_model.offset_point();

  volInt::polyhedron temp_attachment_point_model = *weapon_attachment_point;
  merge_helper_move_model_into_main(
    main_model,
    temp_attachment_point_model,
    attachment_point,
    0.0,
    -1,
    merge_model_type::attachment_point);
}



void m3d_to_wavefront_obj_model::move_weapon_model(
  std::vector<double> new_position,
  double new_angle,
  int weapon_num,
  volInt::polyhedron &weapon_model) const
{
  std::vector<double> weapon_offset = weapon_model.offset_point();
  volInt::rotate_point_by_axis(weapon_offset,
                               new_angle,
                               volInt::rotation_axis::y);
  volInt::vector_minus_self(new_position, weapon_offset);

  // Changing color_id for model.
  weapon_model.set_color_id(c3d::color::string_to_id::weapon, weapon_num);



  // Changing weapon_model's y angle.
  weapon_model.rotate_by_axis(new_angle, volInt::rotation_axis::y);



  // Changing coordinates of all vertices of weapon_model
  // so it will be in the right place.
  // new_position is center coordinates of weapon_model
  // relative to main model center.
  weapon_model.move_model_to_point(new_position);
}



void m3d_to_wavefront_obj_model::add_weapons_to_models_map(
  std::unordered_map<std::string, volInt::polyhedron> &models_map) const
{
  for(int cur_weapon_num = 0;
      cur_weapon_num < m3d::weapon_slot::max_slots;
      ++cur_weapon_num)
  {
    if(flags & m3d_to_obj_flag::extract_nonexistent_weapons ||
       cur_weapon_slot_data[cur_weapon_num].exists)
    {
      std::string cur_model_name =
        wavefront_obj::weapon_obj_name + "_" +
        std::to_string(cur_weapon_num + 1);
      models_map[cur_model_name] = *example_weapon_model;
      move_weapon_model(
        cur_weapon_slot_data[cur_weapon_num].R_slot,
        cur_weapon_slot_data[cur_weapon_num].location_angle_of_slot,
        cur_weapon_num,
        models_map[cur_model_name]);
    }
  }
}



void m3d_to_wavefront_obj_model::add_attachment_point_to_models_map(
  std::unordered_map<std::string, volInt::polyhedron> &models_map,
  point attachment_point_pos) const
{
  models_map[wavefront_obj::attachment_point_obj_name] =
    *weapon_attachment_point;
  models_map[wavefront_obj::attachment_point_obj_name].move_model_to_point(
    attachment_point_pos);
  models_map[wavefront_obj::attachment_point_obj_name].set_color_id(
    c3d::color::string_to_id::attachment_point);
}



void m3d_to_wavefront_obj_model::add_center_of_mass_to_models_map(
  std::unordered_map<std::string, volInt::polyhedron> &models_map,
  point center_of_mass) const
{
  models_map[wavefront_obj::center_of_mass_obj_name] = *center_of_mass_model;
  models_map[wavefront_obj::center_of_mass_obj_name].move_model_to_point(
    center_of_mass);
  models_map[wavefront_obj::center_of_mass_obj_name].set_color_id(
    c3d::color::string_to_id::center_of_mass);
}



void m3d_to_wavefront_obj_model::read_m3d_debris_data(
  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    &debris_models,
  std::vector<volInt::polyhedron> &debris_bound_models,
  std::size_t debris_num)
{
  debris_models[debris_num][wavefront_obj::main_obj_name] =
    read_c3d(c3d::c3d_type::regular);
  debris_bound_models.push_back(read_c3d(c3d::c3d_type::bound));
  if(center_of_mass_model)
  {
    add_center_of_mass_to_models_map(
      debris_models[debris_num],
      debris_models[debris_num][wavefront_obj::main_obj_name].rcm);
  }
}

void m3d_to_wavefront_obj_model::read_m3d_debris_data(
  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    &debris_models,
  std::vector<volInt::polyhedron> &debris_bound_models)
{
  debris_models.assign(n_debris,
                       std::unordered_map<std::string, volInt::polyhedron>());
  // 1 for main model
  std::size_t debris_models_to_reserve = 1;
  if(center_of_mass_model)
  {
    // 1 for center of mass model
    debris_models_to_reserve += 1;
  }
  debris_bound_models.reserve(n_debris);
  for(int debris_num = 0; debris_num < n_debris; ++debris_num)
  {
    debris_models[debris_num].reserve(debris_models_to_reserve);
  }

  for(int debris_num = 0; debris_num < n_debris; ++debris_num)
  {
    read_m3d_debris_data(debris_models,
                         debris_bound_models,
                         debris_num);
  }
}



void m3d_to_wavefront_obj_model::move_debris_to_offset(
  std::unordered_map<std::string, volInt::polyhedron> &debris_model,
  volInt::polyhedron &debris_bound_model)
{
  volInt::polyhedron &main_debris_model =
    debris_model[wavefront_obj::main_obj_name];
  volInt::polyhedron &rcm_debris_model =
    debris_model[wavefront_obj::center_of_mass_obj_name];

  std::vector<double> offset = main_debris_model.offset_point();

  main_debris_model.move_model_to_point_inv_neg_vol(offset);
  rcm_debris_model.move_model_to_point(offset);
  debris_bound_model.move_model_to_point_inv_neg_vol(offset);
}

void m3d_to_wavefront_obj_model::move_debris_to_offset(
  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    &debris_models,
  std::vector<volInt::polyhedron> &debris_bound_models)
{
  for(int debris_num = 0; debris_num < n_debris; ++debris_num)
  {
    move_debris_to_offset(debris_models[debris_num],
                          debris_bound_models[debris_num]);
  }
}



void m3d_to_wavefront_obj_model::save_m3d_debris_data(
  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    &debris_models,
  std::vector<volInt::polyhedron> &debris_bound_models)
{
  for(std::size_t cur_debris = 0; cur_debris < n_debris; ++cur_debris)
  {
    save_c3d_as_wavefront_obj(
      debris_models[cur_debris],
      "_debris_" + std::to_string(cur_debris + 1));
    save_c3d_as_wavefront_obj(
      debris_bound_models[cur_debris],
      "_debris_bound_" + std::to_string(cur_debris + 1));
  }
}



void m3d_to_wavefront_obj_model::read_m3d_weapon_slot(std::size_t slot_id)
{
  cur_weapon_slot_data[slot_id].R_slot =
    read_vec_var_from_m3d_scaled<int, double>(3);
  cur_weapon_slot_data[slot_id].location_angle_of_slot =
    volInt::sicher_angle_to_radians(read_var_from_m3d<int, int>());
  // In weapon_slots_existence only rightmost 3 bits are important.
  // Each bit corresponds to weapon slot from right to left.
  // Example: rightmost bits are "001".
  // In this case weapon 1 exists, weapon 2 and 3 does not exist.
  cur_weapon_slot_data[slot_id].exists =
    ((1 << slot_id) & weapon_slots_existence);
}

void m3d_to_wavefront_obj_model::read_m3d_weapon_slots()
{
  cur_weapon_slot_data =
    std::vector<weapon_slot_data>(m3d::weapon_slot::max_slots);
  for(std::size_t slot_id = 0;
      slot_id < m3d::weapon_slot::max_slots;
      ++slot_id)
  {
    read_m3d_weapon_slot(slot_id);
  }
}



void m3d_to_wavefront_obj_model::save_file_cfg_m3d(
  volInt::polyhedron &main_model,
  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    *debris_models)
{
/*
  std::size_t expected_conf_data_size =
    per_file_cfg_main_overwrite_volume_size +
    per_file_cfg_main_center_of_mass_size +
    per_file_cfg_main_overwrite_inertia_tensor_size +
    per_file_cfg_main_custom_volume_size +
    per_file_cfg_main_custom_inertia_tensor_size;
  if(debris_models && debris_models->size() > 0)
  {
    expected_conf_data_size +=
      per_file_cfg_debris_center_of_mass_size +
      per_file_cfg_debris_overwrite_inertia_tensor_size +
      debris_models->size() * per_file_cfg_debris_custom_inertia_tensor_size;
  }
*/

  std::size_t expected_conf_data_size =
    per_file_cfg_expected_main_size;

  if(debris_models && debris_models->size() > 0)
  {
    expected_conf_data_size +=
      per_file_cfg_expected_debris_header_size +
      debris_models->size() * per_file_cfg_expected_debris_el_size;
  }

  std::string conf_data_to_save;
  conf_data_to_save.reserve(expected_conf_data_size);

  // TEST
//  std::cout << "before conf_data_to_save.capacity(): " <<
//    conf_data_to_save.capacity() << '\n';

  conf_data_to_save.append(
    "# Overwrite volume for main model when custom volume is supplied.\n"
    "# Affects generation of center of mass and inertia tensor.\n"
    "# Used to determine mass. \n"
    "# Mass is used in many calculations and affects ram power.\n");
  if(!(flags & m3d_to_obj_flag::use_custom_volume_by_default))
  {
    conf_data_to_save.push_back('#');
  }
  conf_data_to_save.append("overwrite_volume_main =\n");


  conf_data_to_save.append(
    "\n\n"
    "# Use center of mass mark to get custom center of mass "
      "for main model.\n"
    "# Center of mass is never used in game but used to generate "
      "inertia tensor.\n"
    "# Doesn't change behavior of the object in game if "
      "inertia tensor was overwritten.\n");
  if(!(flags & m3d_to_obj_flag::use_custom_rcm_by_default))
  {
    conf_data_to_save.push_back('#');
  }
  conf_data_to_save.append("custom_center_of_mass_main =\n");


  conf_data_to_save.append(
    "\n\n"
    "# Overwrite inertia tensor matrix for main model "
      "when custom matrix is supplied.\n"
    "# Used in many calculations.\n");
  // Special handling for fishes since they are sensitive to inertia tensor.
  if(!(flags & m3d_to_obj_flag::use_custom_J_by_default) &&
     model_name != "f1")
  {
    conf_data_to_save.push_back('#');
  }
  conf_data_to_save.append("overwrite_inertia_tensor_main =\n");


  conf_data_to_save.append(
    "\n\n"
    "# Custom volume for main model.\n"
    "custom_volume_main = ");
  to_string_precision<double>(
    main_model.volume,
    sprintf_float_per_file_cfg_format,
    conf_data_to_save);


  conf_data_to_save.append(
    "\n\n"
    "# Custom inertia tensor matrix for main model.\n"
    "# Split in 3 rows for convenience.\n");
  for(std::size_t cur_row = 0; cur_row < 3; ++cur_row)
  {
    conf_data_to_save.append(
      "custom_inertia_tensor_main =");
    for(std::size_t cur_row_el = 0; cur_row_el < 3; ++cur_row_el)
    {
      conf_data_to_save.push_back(' ');
      to_string_precision<double>(
        main_model.J[cur_row][cur_row_el],
        sprintf_float_per_file_cfg_format,
        conf_data_to_save);
    }
    conf_data_to_save.push_back('\n');
  }



  if(debris_models && debris_models->size() > 0)
  {
    conf_data_to_save.append(
      "\n\n"
      "# Overwrite volume for debris models "
        "when custom volume is supplied.\n"
      "# Affects generation of center of mass and inertia tensor.\n"
      "# Used to determine mass. \n"
      "# Mass is used in many calculations and affects ram power.\n");
    if(!(flags & m3d_to_obj_flag::use_custom_volume_by_default))
    {
      conf_data_to_save.push_back('#');
    }
    conf_data_to_save.append("overwrite_volume_debris =\n");

    conf_data_to_save.append(
      "\n\n"
      "# Use center of mass mark to get custom center of mass "
        "for debris models.\n"
      "# Center of mass is never used in game but used "
        "to generate inertia tensor.\n"
      "# Doesn't change behavior of the debris in game if inertia tensor "
        "was overwritten.\n");
    if(!(flags & m3d_to_obj_flag::use_custom_rcm_by_default))
    {
      conf_data_to_save.push_back('#');
    }
    conf_data_to_save.append("custom_center_of_mass_debris =\n");

    conf_data_to_save.append(
      "\n\n"
      "# Overwrite inertia tensor matrix for debris models "
        "when custom matrix is supplied.\n"
      "# Used in many calculations.\n");
    if(!(flags & m3d_to_obj_flag::use_custom_J_by_default))
    {
      conf_data_to_save.push_back('#');
    }
    conf_data_to_save.append("overwrite_inertia_tensor_debris =\n");


    for(std::size_t cur_debris = 0,
          debris_models_size = debris_models->size();
        cur_debris < debris_models_size;
        ++cur_debris)
    {
      conf_data_to_save.append(
        "\n\n"
        "# Custom volume for debris model " +
          std::to_string(cur_debris + 1) + ".\n"
        "custom_volume_debris_" +
          std::to_string(cur_debris + 1) + " = ");
      to_string_precision<double>(
        (*debris_models)[cur_debris][wavefront_obj::main_obj_name].volume,
        sprintf_float_per_file_cfg_format,
        conf_data_to_save);


      conf_data_to_save.append(
        "\n\n"
        "# Custom inertia tensor matrix for debris model " +
          std::to_string(cur_debris + 1) + ".\n"
        "# Split in 3 rows for convenience.\n");
      for(std::size_t cur_row = 0; cur_row < 3; ++cur_row)
      {
        conf_data_to_save.append(
          "custom_inertia_tensor_debris_" +
          std::to_string(cur_debris + 1) + " =");
        for(std::size_t cur_row_el = 0; cur_row_el < 3; ++cur_row_el)
        {
          conf_data_to_save.push_back(' ');
          to_string_precision<double>(
            (*debris_models)[cur_debris][wavefront_obj::main_obj_name].
                J[cur_row][cur_row_el],
            sprintf_float_per_file_cfg_format,
            conf_data_to_save);
        }
        conf_data_to_save.push_back('\n');
      }
    }
  }

  // TEST
//  std::cout << "after conf_data_to_save.size(): " <<
//    conf_data_to_save.size() << '\n';

  // save to file
  boost::filesystem::path file_to_save = output_dir_path;
  file_to_save.append(model_name + ".cfg", boost::filesystem::path::codecvt());
  save_file(file_to_save,
            conf_data_to_save,
            file_flag::none,
            output_file_name_error);
}



void m3d_to_wavefront_obj_model::save_file_cfg_a3d(
  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    &animated_models)
{
  std::size_t expected_conf_data_size =
    per_file_cfg_expected_animated_header_size +
    animated_models.size() * per_file_cfg_expected_animated_el_size;

  std::string conf_data_to_save;
  conf_data_to_save.reserve(expected_conf_data_size);

  // TEST
//  std::cout << "before conf_data_to_save.capacity(): " <<
//    conf_data_to_save.capacity() << '\n';

  conf_data_to_save.append(
    "# Overwrite volume for animated models "
      "when custom volume is supplied.\n"
    "# Affects generation of center of mass and inertia tensor.\n"
    "# Used to determine mass. \n"
    "# Mass is used in many calculations and affects ram power.\n");
  if(!(flags & m3d_to_obj_flag::use_custom_volume_by_default))
  {
    conf_data_to_save.push_back('#');
  }
  conf_data_to_save.append("overwrite_volume_animated =\n");


  conf_data_to_save.append(
    "\n\n"
    "# Use center of mass mark to get custom center of mass "
      "for animated models.\n"
    "# Center of mass is never used in game but used "
      "to generate inertia tensor.\n"
    "# Doesn't change behavior of animated model in game if inertia tensor "
      "was overwritten.\n");
  if(!(flags & m3d_to_obj_flag::use_custom_rcm_by_default))
  {
    conf_data_to_save.push_back('#');
  }
  conf_data_to_save.append("custom_center_of_mass_animated =\n");


  conf_data_to_save.append("\n\n"
    "# Overwrite inertia tensor matrix for animated models when "
      "custom matrix is supplied.\n"
    "# Used in many calculations.\n");
  if(!(flags & m3d_to_obj_flag::use_custom_J_by_default))
  {
    conf_data_to_save.push_back('#');
  }
  conf_data_to_save.append("overwrite_inertia_tensor_animated =\n");


  for(std::size_t cur_animated = 0,
        animated_models_size = animated_models.size();
      cur_animated < animated_models_size;
      ++cur_animated)
  {
    conf_data_to_save.append(
      "\n\n"
      "# Custom volume for animated model " +
        std::to_string(cur_animated + 1) + ".\n"
      "custom_volume_animated_" +
        std::to_string(cur_animated + 1) + " = ");
    to_string_precision<double>(
      animated_models[cur_animated][wavefront_obj::main_obj_name].volume,
      sprintf_float_per_file_cfg_format,
      conf_data_to_save);


    conf_data_to_save.append(
      "\n\n"
      "# Custom inertia tensor matrix for animated model " +
        std::to_string(cur_animated + 1) + ".\n"
      "# Split in 3 rows for convenience.\n");
    for(std::size_t cur_row = 0; cur_row < 3; ++cur_row)
    {
      conf_data_to_save.append(
        "custom_inertia_tensor_animated_" +
        std::to_string(cur_animated + 1) + " =");
      for(std::size_t cur_row_el = 0; cur_row_el < 3; ++cur_row_el)
      {
        conf_data_to_save.push_back(' ');
        to_string_precision<double>(
          animated_models[cur_animated][wavefront_obj::main_obj_name].
            J[cur_row][cur_row_el],
          sprintf_float_per_file_cfg_format,
          conf_data_to_save);
      }
      conf_data_to_save.push_back('\n');
    }
  }

  // TEST
//  std::cout << "after conf_data_to_save.size(): " <<
//    conf_data_to_save.size() << '\n';

  // save to file
  boost::filesystem::path file_to_save = output_dir_path;
  file_to_save.append(model_name + ".cfg", boost::filesystem::path::codecvt());
  save_file(file_to_save,
            conf_data_to_save,
            file_flag::none,
            output_file_name_error);
}





std::unordered_map<std::string, double> read_scales_and_copy_game_lst(
  const boost::filesystem::path &lst_filepath,
  const boost::filesystem::path &output_dir_path,
  const std::string &input_file_name_error,
  const std::string &output_file_name_error)
{
  std::unordered_map<std::string, double> scale_sizes;

  std::string game_lst_data =
    read_file(lst_filepath,
              file_flag::binary | file_flag::read_all,
              0,
              0,
              read_all_dummy_size,
              input_file_name_error);

  sicher_cfg_reader cur_cfg_reader(std::move(game_lst_data),
                                   0,
                                   lst_filepath.string(),
                                   input_file_name_error);
  const int max_model = cur_cfg_reader.get_next_value<int>("NumModel");
  const int max_size = cur_cfg_reader.get_next_value<int>("MaxSize");
  // TEST
//std::cout << "max_model: " << max_model << '\n';
//std::cout << "max_size: " << max_size << '\n';
  for(int cur_model = 0; cur_model < max_model; ++cur_model)
  {
    const int model_num = cur_cfg_reader.get_next_value<int>("ModelNum");
    // TEST
//  std::cout << "cur_model: " << cur_model << '\n';
//  std::cout << "model_num: " << model_num << '\n';
    if(cur_model != model_num)
    {
      throw std::runtime_error(
        input_file_name_error + " file " +
        lst_filepath.string() + " unexpected ModelNum " +
        std::to_string(model_num) + ".\n" +
        "Expected " + std::to_string(cur_model) + ".\n" +
        "ModelNum values must be placed " +
        "in order from 0 to (NumModel - 1).\n" +
        "ModelNum is current model, NumModel is total number of models.\n");
    }
    const std::string model_name =
      cur_cfg_reader.get_next_value<std::string>("Name");
    const double scale_size =
      cur_cfg_reader.get_next_value<double>("Size") / max_size;
    const std::string name_id =
      cur_cfg_reader.get_next_value<std::string>("NameID");
    scale_sizes[boost::filesystem::path(model_name).stem().string()] =
      scale_size;
  }

  boost::filesystem::path file_to_save = output_dir_path;
  file_to_save /= lst_filepath.filename();
  save_file(file_to_save,
            cur_cfg_reader.str(),
            file_flag::binary,
            output_file_name_error);

  return scale_sizes;
}

double read_scale_and_copy_prm(
  const boost::filesystem::path &prm_filepath,
  const boost::filesystem::path &output_dir_path,
  const std::string &input_file_name_error,
  const std::string &output_file_name_error)
{
  std::string prm_data =
    read_file(prm_filepath,
              file_flag::binary | file_flag::read_all,
              0,
              0,
              read_all_dummy_size,
              input_file_name_error);

  sicher_cfg_reader cur_cfg_reader(std::move(prm_data),
                                   0,
                                   prm_filepath.string(),
                                   input_file_name_error);

  double scale_size = cur_cfg_reader.get_next_value<double>("scale_size:");

  boost::filesystem::path file_to_save = output_dir_path;
  if(prm_filepath.filename().string() == "default.prm")
  {
    file_to_save /= prm_filepath.filename();
  }
  else
  {
    boost::filesystem::path dir_to_save = file_to_save / prm_filepath.stem();
    boost::filesystem::create_directory(dir_to_save);
    file_to_save = dir_to_save / prm_filepath.filename();
  }
  save_file(file_to_save,
            cur_cfg_reader.str(),
            file_flag::binary,
            output_file_name_error);

  return scale_size;
}



void mechos_m3d_to_wavefront_objs(
  const boost::filesystem::path &m3d_filepath,
  const boost::filesystem::path &output_dir_path,
  const std::string &input_file_name_error,
  const std::string &output_file_name_error,
  const volInt::polyhedron *weapon_model_ptr,
  const volInt::polyhedron *ghost_wheel_model_ptr,
  const volInt::polyhedron *center_of_mass_model_ptr,
  double scale_size,
  unsigned int float_precision_objs,
  bitflag<m3d_to_obj_flag> flags)
{
  m3d_to_wavefront_obj_model cur_vangers_model(
    m3d_filepath,
    output_dir_path,
    input_file_name_error,
    output_file_name_error,
    weapon_model_ptr,
    nullptr,
    ghost_wheel_model_ptr,
    center_of_mass_model_ptr,
    scale_size,
    float_precision_objs,
    flags);
  cur_vangers_model.mechos_m3d_to_wavefront_objs();
}



volInt::polyhedron weapon_m3d_to_wavefront_objs(
  const boost::filesystem::path &m3d_filepath,
  const boost::filesystem::path &output_dir_path,
  const std::string &input_file_name_error,
  const std::string &output_file_name_error,
  const volInt::polyhedron *weapon_attachment_point_model_ptr,
  const volInt::polyhedron *center_of_mass_model_ptr,
  double scale_size,
  unsigned int float_precision_objs,
  bitflag<m3d_to_obj_flag> flags)
{
  flags &= ~m3d_to_obj_flag::extract_nonexistent_weapons;
  m3d_to_wavefront_obj_model cur_vangers_model(
    m3d_filepath,
    output_dir_path,
    input_file_name_error,
    output_file_name_error,
    nullptr,
    weapon_attachment_point_model_ptr,
    nullptr,
    center_of_mass_model_ptr,
    scale_size,
    float_precision_objs,
    flags);
  return cur_vangers_model.weapon_m3d_to_wavefront_objs();
}



void animated_a3d_to_wavefront_objs(
  const boost::filesystem::path &a3d_filepath,
  const boost::filesystem::path &output_dir_path,
  const std::string &input_file_name_error,
  const std::string &output_file_name_error,
  const volInt::polyhedron *center_of_mass_model_ptr,
  double scale_size,
  unsigned int float_precision_objs,
  bitflag<m3d_to_obj_flag> flags)
{
  flags &= ~m3d_to_obj_flag::extract_nonexistent_weapons;
  m3d_to_wavefront_obj_model cur_vangers_model(
    a3d_filepath,
    output_dir_path,
    input_file_name_error,
    output_file_name_error,
    nullptr,
    nullptr,
    nullptr,
    center_of_mass_model_ptr,
    scale_size,
    float_precision_objs,
    flags);
  cur_vangers_model.animated_a3d_to_wavefront_objs();
}



void other_m3d_to_wavefront_objs(
  const boost::filesystem::path &m3d_filepath,
  const boost::filesystem::path &output_dir_path,
  const std::string &input_file_name_error,
  const std::string &output_file_name_error,
  const volInt::polyhedron *center_of_mass_model_ptr,
  double scale_size,
  unsigned int float_precision_objs,
  bitflag<m3d_to_obj_flag> flags)
{
  flags &= ~m3d_to_obj_flag::extract_nonexistent_weapons;
  m3d_to_wavefront_obj_model cur_vangers_model(
    m3d_filepath,
    output_dir_path,
    input_file_name_error,
    output_file_name_error,
    nullptr,
    nullptr,
    nullptr,
    center_of_mass_model_ptr,
    scale_size,
    float_precision_objs,
    flags);
  cur_vangers_model.other_m3d_to_wavefront_objs();
}



} // namespace helpers
} // namespace tractor_converter
