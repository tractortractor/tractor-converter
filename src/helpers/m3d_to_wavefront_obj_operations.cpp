#include "m3d_to_wavefront_obj_operations.hpp"



namespace tractor_converter{
namespace helpers{



m3d_to_wavefront_obj_model::m3d_to_wavefront_obj_model(
  const boost::filesystem::path &input_m3d_path_arg,
  const boost::filesystem::path &output_m3d_path_arg,
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
    input_m3d_path_arg,
    output_m3d_path_arg /
      boost::algorithm::to_lower_copy(input_m3d_path_arg.stem().string()),
    input_file_name_error_arg,
    output_file_name_error_arg,
    example_weapon_model_arg,
    weapon_attachment_point_arg,
    ghost_wheel_model_arg,
    center_of_mass_model_arg),
  float_precision_objs(float_precision_objs_arg),
  flags(flags_arg)
{
  model_name = boost::algorithm::to_lower_copy(input_m3d_path.stem().string());

  scale_size = scale_size_arg;

  m3d_data =
    read_file(input_m3d_path,
              file_flag::binary | file_flag::read_all,
              0,
              0,
              read_all_dummy_size,
              input_file_name_error);
  m3d_data_cur_pos = 0;

  float_precision_objs_string =
    "%." + std::to_string(float_precision_objs) + "f";

  // 2 newline + 1 "v" +
  // 3 "num of coords" *
  //   (1 space + 3 digits left to dot + 1 dot +
  //    float_precision_objs digits right to dot ) +
  // 5
  expected_medium_vertex_size =
    2 + 1 + 3 * (1 + 3 + 1 + float_precision_objs) + 5;

  // +1 because "vn" instead of "v"
  expected_medium_normal_size = 1 + expected_medium_vertex_size;

  non_steer_ghost_wheels_num = 0;
}



void m3d_to_wavefront_obj_model::mechos_m3d_to_wavefront_objs()
{
  boost::filesystem::create_directory(output_m3d_path);

  volInt::polyhedron main_model = read_c3d(c3d::c3d_type::main_of_mechos);

  // IMPORTANT! Header data must be acquired before writing *.c3d to *.obj.
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
    if(flags & m3d_to_obj_flag::extract_bound_model)
    {
      save_m3d_debris_data(&debris_models, &debris_bound_models);
    }
    else
    {
      save_m3d_debris_data(&debris_models);
    }
  }
  if(flags & m3d_to_obj_flag::extract_bound_model)
  {
    c3d_to_wavefront_obj(wavefront_obj::prefix::main_bound,
                         nullptr,
                         c3d::c3d_type::bound);
  }
  else
  {
    read_c3d(c3d::c3d_type::bound);
  }

  weapon_slots_existence = read_var_from_m3d<std::int32_t, int>();
  if(weapon_slots_existence)
  {
    read_m3d_weapon_slots();
  }


  if(n_wheels || weapon_slots_existence)
  {
    merge_helper_reserve_space_in_main(&main_model,
                                       &steer_wheels_models,
                                       &non_steer_ghost_wheels_models);
  }
  if(n_wheels)
  {
    merge_main_model_with_wheels(&main_model,
                                 &steer_wheels_models,
                                 &non_steer_ghost_wheels_models);
  }



  // 1 for main model.
  std::size_t main_models_to_reserve = 1;
  if(weapon_slots_existence && example_weapon_model)
  {
    main_models_to_reserve += m3d::weapon_slot::max_slots;
  }
  if(center_of_mass_model)
  {
    // 1 for center of mass model.
    main_models_to_reserve += 1;
  }

  std::unordered_map<std::string, volInt::polyhedron> main_models;
  main_models.reserve(main_models_to_reserve);
  main_models[wavefront_obj::obj_name::main] = std::move(main_model);

  if(weapon_slots_existence && example_weapon_model)
  {
    add_weapons_to_models_map(main_models);
  }
  if(center_of_mass_model)
  {
    add_center_of_mass_to_models_map(
      main_models,
      main_models[wavefront_obj::obj_name::main].rcm);
  }



  save_c3d_as_wavefront_obj(main_models, wavefront_obj::prefix::main);

  save_file_cfg_m3d(main_models[wavefront_obj::obj_name::main],
                    &debris_models);
}



volInt::polyhedron m3d_to_wavefront_obj_model::weapon_m3d_to_wavefront_objs()
{
  boost::filesystem::create_directory(output_m3d_path);

  volInt::polyhedron main_model = read_c3d(c3d::c3d_type::regular);

  // IMPORTANT! Header data must be acquired before writing *.c3d to *.obj.
  read_m3d_header_data();



  // 1 for main model.
  std::size_t main_models_to_reserve = 1;
  if(weapon_attachment_point)
  {
    // 1 for attachment point model.
    main_models_to_reserve += 1;
  }
  if(center_of_mass_model)
  {
    // 1 for center of mass model.
    main_models_to_reserve += 1;
  }

  std::unordered_map<std::string, volInt::polyhedron> main_models;
  main_models.reserve(main_models_to_reserve);
  main_models[wavefront_obj::obj_name::main] = main_model;



  if(weapon_attachment_point)
  {
    add_attachment_point_to_models_map(
      main_models,
      main_models[wavefront_obj::obj_name::main].offset_point());
  }
  if(center_of_mass_model)
  {
    add_center_of_mass_to_models_map(
      main_models,
      main_models[wavefront_obj::obj_name::main].rcm);
  }
  save_c3d_as_wavefront_obj(main_models, wavefront_obj::prefix::main);

  if(n_wheels)
  {
    throw std::runtime_error(
      input_file_name_error + " file " +
      input_m3d_path.string() +
      ". Non-mechos model has non-zero n_wheels value.");
  }
  if(flags & m3d_to_obj_flag::extract_bound_model)
  {
    c3d_to_wavefront_obj(wavefront_obj::prefix::main_bound,
                         nullptr,
                         c3d::c3d_type::bound);
  }
  else
  {
    read_c3d(c3d::c3d_type::bound);
  }

  weapon_slots_existence = read_var_from_m3d<std::int32_t, int>();
  if(weapon_slots_existence)
  {
    throw std::runtime_error(
      input_file_name_error + " file " +
      input_m3d_path.string() +
      ". Non-mechos model has non-zero weapon_slots_existence.");
  }

  save_file_cfg_m3d(main_models[wavefront_obj::obj_name::main]);


  if(weapon_attachment_point)
  {
    merge_model_with_weapon_attachment_point(main_model);
  }

  return main_model;
}



void m3d_to_wavefront_obj_model::animated_a3d_to_wavefront_objs()
{
  boost::filesystem::create_directory(output_m3d_path);

  // IMPORTANT! Header data must be acquired before writing *.c3d to *.obj.
  read_a3d_header_data();

  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    models(n_models);

  // 1 for main model.
  std::size_t animated_models_to_reserve = 1;
  if(center_of_mass_model)
  {
    // 1 for center of mass model.
    animated_models_to_reserve += 1;
  }

  for(std::size_t cur_animated = 0; cur_animated < n_models; ++cur_animated)
  {
    models[cur_animated].reserve(animated_models_to_reserve);
    models[cur_animated][wavefront_obj::obj_name::main] =
      read_c3d(c3d::c3d_type::regular);

    if(center_of_mass_model)
    {
      add_center_of_mass_to_models_map(
        models[cur_animated],
        models[cur_animated][wavefront_obj::obj_name::main].rcm);
    }
    save_c3d_as_wavefront_obj(models[cur_animated],
                              wavefront_obj::prefix::animated,
                              &cur_animated);
  }
  save_file_cfg_a3d(models);
}



void m3d_to_wavefront_obj_model::other_m3d_to_wavefront_objs()
{
  boost::filesystem::create_directory(output_m3d_path);



  volInt::polyhedron main_model = read_c3d(c3d::c3d_type::regular);

  // IMPORTANT! Header data must be acquired before writing *.c3d to *.obj.
  read_m3d_header_data();



  // 1 for main model.
  std::size_t main_models_to_reserve = 1;
  if(center_of_mass_model)
  {
    // 1 for center of mass model.
    main_models_to_reserve += 1;
  }

  std::unordered_map<std::string, volInt::polyhedron> main_models;
  main_models.reserve(main_models_to_reserve);

  main_models[wavefront_obj::obj_name::main] = std::move(main_model);

  if(center_of_mass_model)
  {
    add_center_of_mass_to_models_map(
      main_models, main_models[wavefront_obj::obj_name::main].rcm);
  }

  save_c3d_as_wavefront_obj(main_models, wavefront_obj::prefix::main);

  if(n_wheels)
  {
    throw std::runtime_error(
      input_file_name_error + " file " +
      input_m3d_path.string() +
      ". Non-mechos model has non-zero n_wheels value.");
  }

  if(flags & m3d_to_obj_flag::extract_bound_model)
  {
    c3d_to_wavefront_obj(wavefront_obj::prefix::main_bound,
                         nullptr,
                         c3d::c3d_type::bound);
  }
  else
  {
    read_c3d(c3d::c3d_type::bound);
  }

  weapon_slots_existence = read_var_from_m3d<std::int32_t, int>();
  if(weapon_slots_existence)
  {
    throw std::runtime_error(
      input_file_name_error + " file " +
      input_m3d_path.string() +
      ". Non-mechos model has non-zero weapon_slots_existence.");
  }

  save_file_cfg_m3d(main_models[wavefront_obj::obj_name::main]);
}





boost::filesystem::path
  m3d_to_wavefront_obj_model::file_prefix_to_path(const std::string &prefix,
                                                  const std::size_t *model_num)
{
  return output_m3d_path / file_prefix_to_filename(prefix, model_num);
}





std::vector<double> m3d_to_wavefront_obj_model::read_vertex()
{
  std::vector<double> vert =
    read_vec_var_from_m3d_scaled<float, double>(volInt::axes_num);
  std::vector<double> discarded_less_preciese_vert =
    read_vec_var_from_m3d_scaled<char, double>(volInt::axes_num);
  int discarded_sort_info = read_var_from_m3d<std::int32_t, int>();
  return vert;
}

void m3d_to_wavefront_obj_model::read_vertices(volInt::polyhedron &model)
{
  for(std::size_t cur_vertex = 0; cur_vertex < model.numVerts; ++cur_vertex)
  {
    model.verts[cur_vertex] = read_vertex();
  }
}



std::vector<double> m3d_to_wavefront_obj_model::read_normal(
  bitflag<normal_flag> flags = normal_flag::sort_info)
{
  std::vector<double> norm =
    volInt::vector_scale(
      wavefront_obj::vector_scale_val,
      read_vec_var_from_m3d<char, double>(volInt::axes_num));
  unsigned char discarded_n_power =
    read_var_from_m3d<unsigned char, unsigned char>();
  if(flags & normal_flag::sort_info)
  {
    int discarded_sort_info = read_var_from_m3d<std::int32_t, int>();
  }
  return norm;
}

void m3d_to_wavefront_obj_model::read_normals(volInt::polyhedron &model)
{
  for(std::size_t norm_ind = 0; norm_ind < model.numVertNorms; ++norm_ind)
  {
    model.vertNorms[norm_ind] = read_normal();
  }
}


volInt::face m3d_to_wavefront_obj_model::read_polygon(
  const volInt::polyhedron &model, std::size_t cur_poly)
{
  int numVerts = read_var_from_m3d<std::int32_t, int>();
  if(model.numVertsPerPoly != numVerts)
  {
    throw std::runtime_error(
      "In " + input_file_name_error +
      " file " + input_m3d_path.string() +
      " polygon " + std::to_string(cur_poly) +
      " at position " +
      std::to_string(m3d_data_cur_pos - sizeof(int)) +
      " has unexpected number of vertices " +
      std::to_string(numVerts) +
      ". Expected " + std::to_string(model.numVertsPerPoly) + ".");
  }
  volInt::face poly(numVerts);

  int discarded_sort_info = read_var_from_m3d<std::int32_t, int>();

  poly.color_id = read_var_from_m3d<std::uint32_t, unsigned int>();
  if(poly.color_id >= c3d::color::string_to_id::max_colors_ids)
  {
    poly.color_id = c3d::color::string_to_id::body;
  }

  unsigned int color_shift = read_var_from_m3d<std::uint32_t, unsigned int>();
  if(color_shift)
  {
    std::cout << "\n\n" <<
      input_file_name_error << " file " <<
      input_m3d_path.string() <<
      ". polygon " << std::to_string(cur_poly) <<
      " at position " +
      std::to_string(m3d_data_cur_pos - sizeof(unsigned int)) +
      " has non-zero color_shift " << std::to_string(color_shift) <<
      ". It is assumed that color_shift of any polygon is always 0." << '\n';
  }


  std::vector<double> discarded_flat_normal =
    read_normal(normal_flag::none);
  std::vector<double> discarded_medium_vert =
    read_vec_var_from_m3d_scaled<char, double>(volInt::axes_num);

  // Note the reverse order of vertices.
  for(std::size_t vert_f_ind = 0, vert_f_ind_r = numVerts - 1;
      vert_f_ind < numVerts;
      ++vert_f_ind, --vert_f_ind_r)
  {
    poly.verts[vert_f_ind_r] = read_var_from_m3d<std::int32_t, int>();
    poly.vertNorms[vert_f_ind_r] =
      read_var_from_m3d<std::int32_t, int>();
  }

  return poly;
}

void m3d_to_wavefront_obj_model::read_polygons(volInt::polyhedron &model)
{
  for(std::size_t cur_poly = 0; cur_poly < model.numFaces; ++cur_poly)
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
  if(cur_c3d_type == c3d::c3d_type::regular ||
     cur_c3d_type == c3d::c3d_type::main_of_mechos)
  {
    expected_vertices_per_poly = c3d::regular_model_vertices_per_polygon;
  }
  else
  {
    expected_vertices_per_poly = c3d::bound_model_vertices_per_polygon;
  }

  int version = read_var_from_m3d<std::int32_t, int>();

  if(version != c3d::version_req)
  {
    throw std::runtime_error(
      input_file_name_error + " file " +
      input_m3d_path.string() + " has unexpected c3d_version " +
      std::to_string(version) + " at position " +
      std::to_string(m3d_data_cur_pos - sizeof(int)) +
      ". Expected " + std::to_string(c3d::version_req) + ".");
  }

  int numVerts =               read_var_from_m3d<std::int32_t, int>();
  int numVertNorms =           read_var_from_m3d<std::int32_t, int>();
  int numFaces =               read_var_from_m3d<std::int32_t, int>();
  int discarded_numVertTotal = read_var_from_m3d<std::int32_t, int>();

  volInt::model_extreme_points discarded_extreme_points;
  discarded_extreme_points.max() =
    read_vec_var_from_m3d_scaled<std::int32_t, double>(volInt::axes_num);
  discarded_extreme_points.min() =
    read_vec_var_from_m3d_scaled<std::int32_t, double>(volInt::axes_num);

  volInt::model_offset offset_point(
    read_vec_var_from_m3d_scaled<std::int32_t, double>(volInt::axes_num));

  double discarded_rmax = read_var_from_m3d_scaled<std::int32_t, double>();

  std::vector<int> discarded_phi_psi_tetta =
    read_vec_var_from_m3d<std::int32_t, int>(volInt::axes_num);

  double volume = read_var_from_m3d_scaled<double, double>(3.0);
  std::vector<double> rcm =
    read_vec_var_from_m3d_scaled<double, double>(volInt::axes_num);
  std::vector<std::vector<double>> J =
    read_nest_vec_var_from_m3d_scaled<double, double>(volInt::axes_num,
                                                      volInt::axes_num,
                                                      5.0);



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
  const std::string &prefix,
  const std::size_t *model_num)
{
  boost::filesystem::path file_to_save =
    file_prefix_to_path(prefix, model_num);

  for(auto &&c3d_model : c3d_models)
  {
    c3d_model.second.bodyColorOffset = body_color_offset;
    c3d_model.second.bodyColorShift =  body_color_shift;
  }

  c3d_models[wavefront_obj::obj_name::main].wavefront_obj_path =
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
  const std::string &prefix,
  const std::size_t *model_num)
{
  std::unordered_map<std::string, volInt::polyhedron> c3d_models
    {{wavefront_obj::obj_name::main, c3d_model}};
  save_c3d_as_wavefront_obj(c3d_models, prefix, model_num);
}



void m3d_to_wavefront_obj_model::c3d_to_wavefront_obj(
  const std::string &prefix,
  const std::size_t *model_num,
  c3d::c3d_type cur_c3d_type)
{
  volInt::polyhedron c3d_model = read_c3d(cur_c3d_type);
  save_c3d_as_wavefront_obj(c3d_model, prefix, model_num);
}



void m3d_to_wavefront_obj_model::read_m3d_header_data()
{
  std::vector<double> discarded_max_point =
    read_vec_var_from_m3d_scaled<std::int32_t, double>(volInt::axes_num);
  double discarded_rmax = read_var_from_m3d_scaled<std::int32_t, double>();

  n_wheels = read_var_from_m3d<std::int32_t, int>();
  n_debris = read_var_from_m3d<std::int32_t, int>();
  body_color_offset = read_var_from_m3d<std::int32_t, int>();
  body_color_shift = read_var_from_m3d<std::int32_t, int>();
}



void m3d_to_wavefront_obj_model::read_a3d_header_data()
{
  n_models = read_var_from_m3d<std::int32_t, int>();

  std::vector<double> discarded_max_point =
    read_vec_var_from_m3d_scaled<std::int32_t, double>(volInt::axes_num);
  double discarded_rmax =
    read_var_from_m3d_scaled<std::int32_t, double>();

  body_color_offset = read_var_from_m3d<std::int32_t, int>();
  body_color_shift = read_var_from_m3d<std::int32_t, int>();
}



void m3d_to_wavefront_obj_model::read_m3d_wheel_data(
  std::vector<volInt::polyhedron> &wheel_models, std::size_t wheel_id)
{
  cur_wheel_data[wheel_id].steer = read_var_from_m3d<std::int32_t, int>();
  cur_wheel_data[wheel_id].r =
    read_vec_var_from_m3d_scaled<double, double>(volInt::axes_num);
  cur_wheel_data[wheel_id].width =
    read_var_from_m3d_scaled<std::int32_t, double>();
  cur_wheel_data[wheel_id].radius =
    read_var_from_m3d_scaled<std::int32_t, double>();
  int discarded_bound_index = read_var_from_m3d<std::int32_t, int>();

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
  for(std::size_t cur_wheel_num = 0; cur_wheel_num < n_wheels; ++cur_wheel_num)
  {
    read_m3d_wheel_data(wheel_models, cur_wheel_num);
  }
  return wheel_models;
}





// Marking wheels as steering/non-steering and ghost/non-ghost.
void m3d_to_wavefront_obj_model::mark_wheels(
  volInt::polyhedron &main_model,
  const std::vector<volInt::polyhedron> &steer_wheels_models)
{
  std::size_t cur_w_data_size = cur_wheel_data.size();
  for(std::size_t wheel_ind = 0; wheel_ind < cur_w_data_size; ++wheel_ind)
  {
    main_model.wheels.insert(wheel_ind);
  }

  // Marking steering wheels.
  for(std::size_t wheel_ind = 0; wheel_ind < cur_w_data_size; ++wheel_ind)
  {
    if(cur_wheel_data[wheel_ind].steer)
    {
      main_model.wheels_steer.insert(wheel_ind);
    }
    else
    {
      main_model.wheels_non_steer.insert(wheel_ind);
    }
  }

  // Marking ghost wheels.
  non_steer_ghost_wheels_num = 0;
  for(std::size_t wheel_ind = 0; wheel_ind < cur_w_data_size; ++wheel_ind)
  {
    if(cur_wheel_data[wheel_ind].steer)
    {
      if(steer_wheels_models.
           at(cur_wheel_data[wheel_ind].wheel_model_index).numFaces ==
         0)
      {
        cur_wheel_data[wheel_ind].ghost = 1;
        main_model.wheels_ghost.insert(wheel_ind);
      }
      else
      {
        cur_wheel_data[wheel_ind].ghost = 0;
        main_model.wheels_non_ghost.insert(wheel_ind);
      }
    }
    else
    {
      cur_wheel_data[wheel_ind].ghost = 1;
      main_model.wheels_ghost.insert(wheel_ind);
      ++non_steer_ghost_wheels_num;
    }
  }
}



// Copying ghost wheel model and scaling it to specified width and radius.
volInt::polyhedron
  m3d_to_wavefront_obj_model::get_ghost_wheels_helper_generate_wheel(
    int wheel_id)
{
  volInt::polyhedron cur_ghost_wheel = *ghost_wheel_model;

  cur_ghost_wheel.get_extreme_points();
  double given_width = cur_ghost_wheel.xmax() - cur_ghost_wheel.xmin();
  double width_multiplier = cur_wheel_data[wheel_id].width / given_width;
  double given_radius =
    (cur_ghost_wheel.zmax() - cur_ghost_wheel.zmin()) / 2;
  double radius_multiplier = cur_wheel_data[wheel_id].radius / given_radius;

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

  return cur_ghost_wheel;
}



void m3d_to_wavefront_obj_model::get_ghost_wheels(
  const volInt::polyhedron &main_model,
  std::vector<volInt::polyhedron> &steer_wheels_models,
  std::vector<volInt::polyhedron> &non_steer_ghost_wheels_models)
{
  non_steer_ghost_wheels_models.reserve(non_steer_ghost_wheels_num);

  for(std::size_t wheel_ind = 0, cur_wheel_data_size = cur_wheel_data.size();
      wheel_ind < cur_wheel_data_size;
      ++wheel_ind)
  {
    if(cur_wheel_data[wheel_ind].steer)
    {
      if(cur_wheel_data[wheel_ind].ghost)
      {
        steer_wheels_models[cur_wheel_data[wheel_ind].wheel_model_index] =
          get_ghost_wheels_helper_generate_wheel(wheel_ind);
      }
    }
    else
    {
      if(cur_wheel_data[wheel_ind].ghost)
      {
        non_steer_ghost_wheels_models.push_back(
          get_ghost_wheels_helper_generate_wheel(wheel_ind));
        cur_wheel_data[wheel_ind].wheel_model_index =
          non_steer_ghost_wheels_models.size() - 1;
      }
    }
  }
}



void m3d_to_wavefront_obj_model::move_weapon_model(
  std::vector<double> new_position,
  double new_angle,
  int weapon_id,
  volInt::polyhedron &weapon_model) const
{
  std::vector<double> weapon_offset = weapon_model.offset_point();
  volInt::rotate_point_by_axis(weapon_offset,
                               new_angle,
                               volInt::rotation_axis::y);
  volInt::vector_minus_self(new_position, weapon_offset);

  // Changing color_id for model.
  weapon_model.set_color_id(c3d::color::string_to_id::invalid_color_id,
                            volInt::invalid::wheel_id,
                            weapon_id);



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
  for(std::size_t cur_weapon_id = 0;
      cur_weapon_id < m3d::weapon_slot::max_slots;
      ++cur_weapon_id)
  {
    if(flags & m3d_to_obj_flag::extract_nonexistent_weapons ||
       cur_weapon_slot_data[cur_weapon_id].exists)
    {
      std::string cur_model_name =
        wavefront_obj::obj_name::weapon + "_" +
        std::to_string(cur_weapon_id + 1);
      models_map[cur_model_name] = *example_weapon_model;
      move_weapon_model(
        cur_weapon_slot_data[cur_weapon_id].R_slot,
        cur_weapon_slot_data[cur_weapon_id].location_angle_of_slot,
        cur_weapon_id,
        models_map[cur_model_name]);
    }
  }
}



void m3d_to_wavefront_obj_model::add_attachment_point_to_models_map(
  std::unordered_map<std::string, volInt::polyhedron> &models_map,
  point attachment_point_pos) const
{
  models_map[wavefront_obj::obj_name::attachment_point] =
    *weapon_attachment_point;
  models_map[wavefront_obj::obj_name::attachment_point].move_model_to_point(
    attachment_point_pos);
  models_map[wavefront_obj::obj_name::attachment_point].set_color_id(
    c3d::color::string_to_id::attachment_point);
}



void m3d_to_wavefront_obj_model::add_center_of_mass_to_models_map(
  std::unordered_map<std::string, volInt::polyhedron> &models_map,
  point center_of_mass) const
{
  models_map[wavefront_obj::obj_name::center_of_mass] = *center_of_mass_model;
  models_map[wavefront_obj::obj_name::center_of_mass].move_model_to_point(
    center_of_mass);
  models_map[wavefront_obj::obj_name::center_of_mass].set_color_id(
    c3d::color::string_to_id::center_of_mass);
}



void m3d_to_wavefront_obj_model::read_m3d_debris_data(
  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    &debris_models,
  std::vector<volInt::polyhedron> &debris_bound_models,
  std::size_t debris_num)
{
  debris_models[debris_num][wavefront_obj::obj_name::main] =
    read_c3d(c3d::c3d_type::regular);
  debris_bound_models.push_back(read_c3d(c3d::c3d_type::bound));
  if(center_of_mass_model)
  {
    add_center_of_mass_to_models_map(
      debris_models[debris_num],
      debris_models[debris_num][wavefront_obj::obj_name::main].rcm);
  }
}

void m3d_to_wavefront_obj_model::read_m3d_debris_data(
  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    &debris_models,
  std::vector<volInt::polyhedron> &debris_bound_models)
{
  debris_models.assign(n_debris,
                       std::unordered_map<std::string, volInt::polyhedron>());
  // 1 for main model.
  std::size_t debris_models_to_reserve = 1;
  if(center_of_mass_model)
  {
    // 1 for center of mass model.
    debris_models_to_reserve += 1;
  }
  debris_bound_models.reserve(n_debris);
  for(std::size_t debris_num = 0; debris_num < n_debris; ++debris_num)
  {
    debris_models[debris_num].reserve(debris_models_to_reserve);
  }

  for(std::size_t debris_num = 0; debris_num < n_debris; ++debris_num)
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
    debris_model[wavefront_obj::obj_name::main];

  std::vector<double> offset = main_debris_model.offset_point();

  main_debris_model.move_model_to_point_inv_neg_vol(offset);
  if(center_of_mass_model)
  {
    volInt::polyhedron &rcm_debris_model =
      debris_model[wavefront_obj::obj_name::center_of_mass];
    rcm_debris_model.move_model_to_point(offset);
  }
  debris_bound_model.move_model_to_point_inv_neg_vol(offset);
}

void m3d_to_wavefront_obj_model::move_debris_to_offset(
  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    &debris_models,
  std::vector<volInt::polyhedron> &debris_bound_models)
{
  for(std::size_t debris_num = 0; debris_num < n_debris; ++debris_num)
  {
    move_debris_to_offset(debris_models[debris_num],
                          debris_bound_models[debris_num]);
  }
}



void m3d_to_wavefront_obj_model::save_m3d_debris_data(
  std::vector<std::unordered_map<std::string, volInt::polyhedron>>
    *debris_models,
  std::vector<volInt::polyhedron> *debris_bound_models)
{
  for(std::size_t cur_debris = 0; cur_debris < n_debris; ++cur_debris)
  {
    save_c3d_as_wavefront_obj((*debris_models)[cur_debris],
                              wavefront_obj::prefix::debris,
                              &cur_debris);
  }
  if(debris_bound_models)
  {
    for(std::size_t cur_debris = 0; cur_debris < n_debris; ++cur_debris)
    {
      save_c3d_as_wavefront_obj((*debris_bound_models)[cur_debris],
                                wavefront_obj::prefix::debris_bound,
                                &cur_debris);
    }
  }
}



void m3d_to_wavefront_obj_model::read_m3d_weapon_slot(std::size_t slot_id)
{
  cur_weapon_slot_data[slot_id].R_slot =
    read_vec_var_from_m3d_scaled<std::int32_t, double>(volInt::axes_num);
  cur_weapon_slot_data[slot_id].location_angle_of_slot =
    volInt::sicher_angle_to_radians(read_var_from_m3d<std::int32_t, int>());
  // In weapon_slots_existence only rightmost 3 bits are important.
  // Each bit corresponds to weapon slot from right to left.
  // Example: rightmost bits are "001".
  // In that case, weapon 1 exists, weapon 2 and 3 do not exist.
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

  conf_data_to_save.append(
    "# Overwrite volume for main model when custom volume is supplied.\n"
    "# Affects generation of center of mass and inertia tensor.\n"
    "# Used to determine mass. \n"
    "# Mass is used in many calculations and affects ram power.\n");
  if(!(flags & m3d_to_obj_flag::use_custom_volume_by_default))
  {
    conf_data_to_save.push_back('#');
  }
  conf_data_to_save.append(
    option::per_file::name::overwrite_volume_main + " =\n");


  conf_data_to_save.append(
    "\n\n"
    "# Use center of mass mark to get custom center of mass "
      "for main model.\n"
    "# Center of mass is never used in the game "
      "but used to generate inertia tensor.\n"
    "# Doesn't change behavior of the object in the game "
      "if inertia tensor was overwritten.\n");
  if(!(flags & m3d_to_obj_flag::use_custom_rcm_by_default))
  {
    conf_data_to_save.push_back('#');
  }
  conf_data_to_save.append(
    option::per_file::name::custom_center_of_mass_main + " =\n");


  conf_data_to_save.append(
    "\n\n"
    "# Overwrite inertia tensor matrix for main model "
      "when custom matrix is supplied.\n"
    "# Used in many calculations.\n");
  if(!(flags & m3d_to_obj_flag::use_custom_J_by_default) &&
     !inertia_tensor_sensitive_models.count(model_name))
  {
    conf_data_to_save.push_back('#');
  }
  conf_data_to_save.append(
    option::per_file::name::overwrite_inertia_tensor_main + " =\n");


  conf_data_to_save.append(
    "\n\n"
    "# Custom volume for main model.\n" +
    option::per_file::name::custom_volume_main + " = ");
  to_string_precision<double>(
    main_model.volume,
    sprintf_float_per_file_cfg_format,
    conf_data_to_save);


  conf_data_to_save.append(
    "\n\n"
    "# Custom inertia tensor matrix for main model.\n"
    "# Split into 3 rows for convenience.\n");
  for(std::size_t cur_row = 0; cur_row < volInt::axes_num; ++cur_row)
  {
    conf_data_to_save.append(
      option::per_file::name::custom_inertia_tensor_main + " =");
    for(std::size_t row_el = 0; row_el < volInt::axes_num; ++row_el)
    {
      conf_data_to_save.push_back(' ');
      to_string_precision<double>(
        main_model.J[cur_row][row_el],
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
    conf_data_to_save.append(
      option::per_file::name::overwrite_volume_debris + " =\n");

    conf_data_to_save.append(
      "\n\n"
      "# Use center of mass mark to get custom center of mass "
        "for debris models.\n"
      "# Center of mass is never used in the game "
        "but used to generate inertia tensor.\n"
      "# Doesn't change behavior of the debris in the game "
        "if inertia tensor was overwritten.\n");
    if(!(flags & m3d_to_obj_flag::use_custom_rcm_by_default))
    {
      conf_data_to_save.push_back('#');
    }
    conf_data_to_save.append(
      option::per_file::name::custom_center_of_mass_debris + " =\n");

    conf_data_to_save.append(
      "\n\n"
      "# Overwrite inertia tensor matrix for debris models "
        "when custom matrix is supplied.\n"
      "# Used in many calculations.\n");
    if(!(flags & m3d_to_obj_flag::use_custom_J_by_default))
    {
      conf_data_to_save.push_back('#');
    }
    conf_data_to_save.append(
      option::per_file::name::overwrite_inertia_tensor_debris + " =\n");


    for(std::size_t cur_debris = 0,
          debris_models_size = debris_models->size();
        cur_debris < debris_models_size;
        ++cur_debris)
    {
      conf_data_to_save.append(
        "\n\n"
        "# Custom volume for debris model " +
          std::to_string(cur_debris + 1) + ".\n" +
        option::per_file::name::custom_volume_debris + "_" +
          std::to_string(cur_debris + 1) + " = ");
      to_string_precision<double>(
        (*debris_models)[cur_debris][wavefront_obj::obj_name::main].volume,
        sprintf_float_per_file_cfg_format,
        conf_data_to_save);


      conf_data_to_save.append(
        "\n\n"
        "# Custom inertia tensor matrix for debris model " +
          std::to_string(cur_debris + 1) + ".\n"
        "# Split into 3 rows for convenience.\n");
      for(std::size_t cur_row = 0; cur_row < volInt::axes_num; ++cur_row)
      {
        conf_data_to_save.append(
          option::per_file::name::custom_inertia_tensor_debris + "_" +
          std::to_string(cur_debris + 1) + " =");
        for(std::size_t row_el = 0; row_el < volInt::axes_num; ++row_el)
        {
          conf_data_to_save.push_back(' ');
          to_string_precision<double>(
            (*debris_models)[cur_debris][wavefront_obj::obj_name::main].
                J[cur_row][row_el],
            sprintf_float_per_file_cfg_format,
            conf_data_to_save);
        }
        conf_data_to_save.push_back('\n');
      }
    }
  }

  boost::filesystem::path file_to_save = output_m3d_path;
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
  conf_data_to_save.append(
    option::per_file::name::overwrite_volume_animated + " =\n");


  conf_data_to_save.append(
    "\n\n"
    "# Use center of mass mark to get custom center of mass "
      "for animated models.\n"
    "# Center of mass is never used in the game "
      "but used to generate inertia tensor.\n"
    "# Doesn't change behavior of animated model in the game "
      "if inertia tensor was overwritten.\n");
  if(!(flags & m3d_to_obj_flag::use_custom_rcm_by_default))
  {
    conf_data_to_save.push_back('#');
  }
  conf_data_to_save.append(
    option::per_file::name::custom_center_of_mass_animated + " =\n");


  conf_data_to_save.append("\n\n"
    "# Overwrite inertia tensor matrix for animated models "
      "when custom matrix is supplied.\n"
    "# Used in many calculations.\n");
  if(!(flags & m3d_to_obj_flag::use_custom_J_by_default))
  {
    conf_data_to_save.push_back('#');
  }
  conf_data_to_save.append(
    option::per_file::name::overwrite_inertia_tensor_animated + " =\n");


  for(std::size_t cur_animated = 0,
        animated_models_size = animated_models.size();
      cur_animated < animated_models_size;
      ++cur_animated)
  {
    conf_data_to_save.append(
      "\n\n"
      "# Custom volume for animated model " +
        std::to_string(cur_animated + 1) + ".\n" +
      option::per_file::name::custom_volume_animated + "_" +
        std::to_string(cur_animated + 1) + " = ");
    to_string_precision<double>(
      animated_models[cur_animated][wavefront_obj::obj_name::main].volume,
      sprintf_float_per_file_cfg_format,
      conf_data_to_save);


    conf_data_to_save.append(
      "\n\n"
      "# Custom inertia tensor matrix for animated model " +
        std::to_string(cur_animated + 1) + ".\n"
      "# Split into 3 rows for convenience.\n");
    for(std::size_t cur_row = 0; cur_row < volInt::axes_num; ++cur_row)
    {
      conf_data_to_save.append(
        option::per_file::name::custom_inertia_tensor_animated + "_" +
        std::to_string(cur_animated + 1) + " =");
      for(std::size_t row_el = 0; row_el < volInt::axes_num; ++row_el)
      {
        conf_data_to_save.push_back(' ');
        to_string_precision<double>(
          animated_models[cur_animated][wavefront_obj::obj_name::main].
            J[cur_row][row_el],
          sprintf_float_per_file_cfg_format,
          conf_data_to_save);
      }
      conf_data_to_save.push_back('\n');
    }
  }

  boost::filesystem::path file_to_save = output_m3d_path;
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
                                   lst_filepath.string(),
                                   input_file_name_error);
  cur_cfg_reader.fix_game_lst_paths();

  const int max_model =
    cur_cfg_reader.get_next_value<int>(van_cfg_key::game_lst::NumModel);
  const int max_size =
    cur_cfg_reader.get_next_value<int>(van_cfg_key::game_lst::MaxSize);
  for(std::size_t cur_model = 0; cur_model < max_model; ++cur_model)
  {
    const int model_num =
      cur_cfg_reader.get_next_value<int>(van_cfg_key::game_lst::ModelNum);
    if(cur_model != model_num)
    {
      throw std::runtime_error(
        input_file_name_error + " file " +
        lst_filepath.string() + " unexpected " +
        van_cfg_key::game_lst::ModelNum + " " +
        std::to_string(model_num) + ".\n" +
        "Expected " + std::to_string(cur_model) + ".\n" +
        van_cfg_key::game_lst::ModelNum +
        " values must be placed " +
        "in order from 0 to (" + van_cfg_key::game_lst::NumModel + " - 1).\n" +
        van_cfg_key::game_lst::ModelNum +
        " is current model, " + van_cfg_key::game_lst::NumModel +
        " is total number of models.\n");
    }
    const std::string model_game_path =
      cur_cfg_reader.get_next_value<std::string>(van_cfg_key::game_lst::Name);
    const double scale_size =
      cur_cfg_reader.get_next_value<double>(van_cfg_key::game_lst::Size) /
      max_size;
    const std::string name_id =
      cur_cfg_reader.get_next_value<std::string>(
        van_cfg_key::game_lst::NameID);
    scale_sizes[boost::filesystem::path(model_game_path).stem().string()] =
      scale_size;
  }

  boost::filesystem::path file_to_save =
    output_dir_path /
    boost::algorithm::to_lower_copy(lst_filepath.filename().string());
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
                                   prm_filepath.string(),
                                   input_file_name_error);

  double scale_size =
    cur_cfg_reader.get_next_value<double>(van_cfg_key::prm::scale_size);

  boost::filesystem::path file_to_save;
  boost::filesystem::path prm_filename_lowercase =
    boost::algorithm::to_lower_copy(prm_filepath.filename().string());
  if(prm_filename_lowercase.string() == file::default_prm)
  {
    file_to_save = output_dir_path / prm_filename_lowercase;
  }
  else
  {
    boost::filesystem::path dir_to_save =
      output_dir_path / prm_filename_lowercase.stem();
    boost::filesystem::create_directory(dir_to_save);
    file_to_save = dir_to_save / prm_filename_lowercase;
  }
  save_file(file_to_save,
            cur_cfg_reader.str(),
            file_flag::binary,
            output_file_name_error);

  return scale_size;
}



void mechos_m3d_to_wavefront_objs(
  const boost::filesystem::path &m3d_filepath,
  const boost::filesystem::path &output_m3d_path,
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
    output_m3d_path,
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
  const boost::filesystem::path &output_m3d_path,
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
    output_m3d_path,
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
  const boost::filesystem::path &output_m3d_path,
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
    output_m3d_path,
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
  const boost::filesystem::path &output_m3d_path,
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
    output_m3d_path,
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
