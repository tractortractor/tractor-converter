#include "wavefront_obj_to_m3d_operations.hpp"



namespace tractor_converter{
namespace helpers{



void get_extreme_radius(double &cur_extreme, double radius, point offset)
{
  double offset_radius =
    std::sqrt(offset[0]*offset[0] + offset[1]*offset[1] + offset[2]*offset[2]);
  double end_radius = std::abs(radius) + offset_radius;
  if(cur_extreme < end_radius)
  {
    cur_extreme = end_radius;
  }
}



// Get radius of bound sphere around attachment point.
double get_weapons_bound_sphere_radius(
  const std::unordered_map<std::string, volInt::polyhedron> &weapons_models)
{
  double max_radius = 0.0;
  for(const auto &weapon_model : weapons_models)
  {
    for(const auto &vert : weapon_model.second.verts)
    {
      double cur_vert_length =
        volInt::vector_length_between(vert,
                                      weapon_model.second.offset_point());
      if(max_radius < cur_vert_length)
      {
        max_radius = cur_vert_length;
      }
    }
  }
  return max_radius;
/*
  double max_radius = 0.0;
  for(const auto &weapon_model : weapons_models)
  {
    if(max_radius < weapon_model.second.rmax)
    {
      max_radius = weapon_model.second.rmax;
    }
  }
  return max_radius;
*/
}



template<> double parse_per_file_cfg_option<double>(
  const std::string &input)
{
  char* cur_pos = const_cast<char*>(&input[0]);
  return std::strtod(cur_pos, NULL);
}

template<> std::vector<double> parse_per_file_cfg_multiple_options<double>(
  const std::string &input)
{
  std::vector<double> to_return;
  char* cur_pos = const_cast<char*>(&input[0]);
  while(*cur_pos != '\0')
  {
    to_return.push_back(std::strtod(cur_pos, &cur_pos));
  }
  return to_return;
}

template<> std::vector<double> parse_per_file_cfg_multiple_options<double>(
  const std::vector<std::string> &input)
{
  std::vector<double> to_return;
  for(const auto &cur_line : input)
  {
    std::vector<double> per_line_values;
    per_line_values = parse_per_file_cfg_multiple_options<double>(cur_line);
    to_return.insert(to_return.end(),
                     per_line_values.begin(),
                     per_line_values.end());
  }

  return to_return;
}



wavefront_obj_to_m3d_model::wavefront_obj_to_m3d_model(
  const boost::filesystem::path &input_file_path_arg,
  const boost::filesystem::path &output_dir_path_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const volInt::polyhedron *example_weapon_model_arg,
  const volInt::polyhedron *weapon_attachment_point_arg,
  const volInt::polyhedron *center_of_mass_model_arg,
  double max_weapons_radius_arg,
  unsigned int c3d_default_material_id_arg,
  std::unordered_map<std::string, double> *non_mechos_scale_sizes_arg)
: vangers_model(input_file_path_arg,
    output_dir_path_arg,
    input_file_name_error_arg,
    output_file_name_error_arg,
    example_weapon_model_arg,
    weapon_attachment_point_arg,
    nullptr,
    center_of_mass_model_arg),
  max_weapons_radius(max_weapons_radius_arg),
  c3d_default_material_id(c3d_default_material_id_arg),
  non_mechos_scale_sizes(non_mechos_scale_sizes_arg)
{
  model_name = input_file_path_arg.filename().string();

  prm_scale_size = 0.0;
}



void wavefront_obj_to_m3d_model::mechos_wavefront_objs_to_m3d()
{
  volInt::polyhedron cur_main_model =
    read_obj_prefix("main", c3d::c3d_type::regular);
  volInt::polyhedron cur_main_bound_model =
    read_obj_prefix("main_bound", c3d::c3d_type::bound);

  std::deque<volInt::polyhedron> debris_models =
    read_objs_with_prefix("debris", c3d::c3d_type::regular);
  std::deque<volInt::polyhedron> debris_bound_models =
    read_objs_with_prefix("debris_bound", c3d::c3d_type::bound);

  // Must be called before call to remove_polygons() and get_m3d_header_data()
  read_file_cfg_m3d(cur_main_model, &debris_models);



  // TEST
//std::cout << '\n';
//std::cout << "getting weapons of model: " << input_file_path << '\n';
  get_weapons_data(cur_main_model);
  get_wheels_data(cur_main_model);
  get_debris_data(debris_models, debris_bound_models);

  std::unordered_map<int, volInt::polyhedron> wheels_models =
    get_wheels_steer(cur_main_model);


  // Must be called before call to get_m3d_scale_size()
  remove_polygons(cur_main_model, remove_polygons_model::mechos);
  for(auto &&debris_model : debris_models)
  {
    remove_polygons(debris_model, remove_polygons_model::regular);
  }

    // TEST
//  std::cout << "first time!" << '\n';
//  std::cout << "model:" << model_name << '\n';


  get_m3d_scale_size(&cur_main_model,
                     &cur_main_bound_model,
                     &wheels_models,
                     &debris_models,
                     &debris_bound_models);

  get_m3d_header_data(&cur_main_model,
                      &cur_main_bound_model,
                      &wheels_models,
                      &debris_models,
                      &debris_bound_models);



  std::size_t m3d_file_size = get_m3d_file_size(&cur_main_model,
                                                &cur_main_bound_model,
                                                &wheels_models,
                                                &debris_models,
                                                &debris_bound_models);



  // Allocating enough space for *.m3d file
  m3d_data = std::string(m3d_file_size, '\0');
  m3d_data_cur_pos = 0;

  write_c3d(cur_main_model);
  write_m3d_header_data();

  if(n_wheels)
  {
    write_m3d_wheels_data(wheels_models);
  }
  if(n_debris)
  {
    write_m3d_debris_data(debris_models, debris_bound_models);
  }
  write_c3d(cur_main_bound_model);


    // TEST
//  std::cout << "model:" << model_name << '\n';
//  std::cout << "weapon_slots_existence:" << weapon_slots_existence << '\n';


  write_var_to_m3d<int>(weapon_slots_existence);
  if(weapon_slots_existence)
  {
    write_m3d_weapon_slots();
  }


  boost::filesystem::path file_to_save = output_dir_path;
  file_to_save.append(model_name + ".m3d", boost::filesystem::path::codecvt());
  helpers::save_file(file_to_save,
                     m3d_data,
                     TRACTOR_CONVERTER_BINARY,
                     output_file_name_error);



  boost::filesystem::path prm_file_input = input_file_path;
  prm_file_input.append(model_name + ".prm",
                        boost::filesystem::path::codecvt());

  boost::filesystem::path prm_file_output = output_dir_path;
  prm_file_output.append(model_name + ".prm",
                         boost::filesystem::path::codecvt());

  create_prm(prm_file_input,
             prm_file_output,
             input_file_name_error,
             output_file_name_error,
             prm_scale_size);



  // TEST
  /*
  std::cout << "second time!" << '\n';
  std::cout << "model:" << model_name << '\n';
  std::cout << "scale_size: " << scale_size << '\n';

  volInt::vector_multiply_self(cur_main_model.rcm, scale_size);

  for(auto &&vert : cur_main_model.verts)
  {
    volInt::vector_multiply_self(vert, scale_size);
  }
//std::cout << model_name << " main mechos models after scaling." << '\n';
  cur_main_model.faces_calc_params();
  cur_main_model.calculate_c3d_properties();
  */



  // TEST
//std::cout << '\n';
//std::cout << "scale_size of model: " <<
//  input_file_path << " is " << scale_size << '\n';



  // TEST
//std::cout << '\n';
//std::cout << "model: " << input_file_path << '\n';
//for(std::size_t cur_weapon = 0;
//    cur_weapon < m3d::weapon_slot::max_slots;
//    ++cur_weapon)
//{
//  std::vector<double> R_slots;
//  double location_angle_of_slots;
//  std::cout <<
//    "cur_weapon_slot_data[" << cur_weapon << "].location_angle_of_slots" <<
//    cur_weapon_slot_data[cur_weapon].location_angle_of_slots << '\n';
//  for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
//  {
//    std::cout <<
//      "cur_weapon_slot_data[" << cur_weapon <<
//        "].R_slots[" << cur_coord << "]" <<
//      cur_weapon_slot_data[cur_weapon].R_slots[cur_coord] << '\n';
//  }
//}

  // TEST
  /*
  std::unordered_map<std::string, volInt::polyhedron> cur_main_model_map
    {{wavefront_obj::main_obj_name, cur_main_model}};
  save_volInt_as_wavefront_obj(
    cur_main_model_map,
    output_dir_path.parent_path() / (model_name + "_test.obj"),
    "test");
  for(const auto &wheel_model : wheels_models)
  {
    std::unordered_map<std::string, volInt::polyhedron> cur_wheel_model_map
      {{wavefront_obj::main_obj_name, wheel_model.second}};
    save_volInt_as_wavefront_obj(
      cur_wheel_model_map,
      output_dir_path.parent_path() /
        (model_name +
           "_wheel_steer_" +
           std::to_string(wheel_model.first) +
           ".obj"),
      "test");
  }
  */
}



volInt::polyhedron wavefront_obj_to_m3d_model::weapon_wavefront_objs_to_m3d()
{
  volInt::polyhedron cur_main_model =
    read_obj_prefix("main", c3d::c3d_type::regular);
  volInt::polyhedron cur_main_bound_model =
    read_obj_prefix("main_bound", c3d::c3d_type::bound);

  // Must be called before call to remove_polygons() and get_m3d_header_data()
  read_file_cfg_m3d(cur_main_model);



  get_attachment_point(cur_main_model);


  // Must be called before call to get_m3d_scale_size()
  remove_polygons(cur_main_model, remove_polygons_model::weapon);


  get_m3d_scale_size(&cur_main_model, &cur_main_bound_model);

  get_m3d_header_data(&cur_main_model, &cur_main_bound_model);



  std::size_t m3d_file_size =
    get_m3d_file_size(&cur_main_model, &cur_main_bound_model);



  // Allocating enough space for *.m3d file
  m3d_data = std::string(m3d_file_size, '\0');
  m3d_data_cur_pos = 0;

  write_c3d(cur_main_model);
  write_m3d_header_data();
  write_c3d(cur_main_bound_model);
  write_var_to_m3d<int>(weapon_slots_existence);



  boost::filesystem::path file_to_save = output_dir_path;
  file_to_save.append(model_name + ".m3d", boost::filesystem::path::codecvt());
  helpers::save_file(file_to_save,
                     m3d_data,
                     TRACTOR_CONVERTER_BINARY,
                     output_file_name_error);



  // TEST
//std::cout << '\n';
//std::cout << "scale_size of model: " <<
//  input_file_path << " is " << scale_size << '\n';

  // TEST
//std::cout << '\n';
//std::cout << "model_name: " << model_name << '\n';
//std::cout << "x_off: " << cur_main_model.x_off() << '\n';
//std::cout << "y_off: " << cur_main_model.y_off() << '\n';
//std::cout << "z_off: " << cur_main_model.z_off() << '\n';
//save_volInt_as_wavefront_obj(
//  cur_main_model,
//  output_dir_path.parent_path() / (model_name + "_test.obj"),
//  "test");


  // TEST
//cur_main_model.max() = { 1,  1,  1};
//cur_main_model.min() = {-1, -1, -1};



  return cur_main_model;
}



void wavefront_obj_to_m3d_model::animated_wavefront_objs_to_a3d()
{
  std::deque<volInt::polyhedron> animated_models =
    read_objs_with_prefix("", c3d::c3d_type::regular);


  // Must be called before call to remove_polygons() and get_a3d_header_data()
  read_file_cfg_a3d(animated_models);



  // Must be called before call to get_a3d_scale_size()
  for(auto &&animated_model : animated_models)
  {
    remove_polygons(animated_model, remove_polygons_model::regular);
  }

  get_a3d_scale_size(&animated_models);

  get_a3d_header_data(&animated_models);



  std::size_t a3d_file_size = get_a3d_file_size(&animated_models);

  // Allocating enough space for *.a3d file
  m3d_data = std::string(a3d_file_size, '\0');
  m3d_data_cur_pos = 0;

  write_a3d_header_data();
  for(int i = 0; i < n_models; ++i)
  {
    write_c3d(animated_models[i]);
  }



  boost::filesystem::path file_to_save = output_dir_path;
  file_to_save.append(model_name + ".a3d", boost::filesystem::path::codecvt());
  helpers::save_file(file_to_save,
                     m3d_data,
                     TRACTOR_CONVERTER_BINARY,
                     output_file_name_error);
  // TEST
//std::cout << '\n';
//std::cout << "scale_size of model: " << input_file_path <<
//  " is " << scale_size << '\n';
}



void wavefront_obj_to_m3d_model::other_wavefront_objs_to_m3d()
{
  volInt::polyhedron cur_main_model =
    read_obj_prefix("main", c3d::c3d_type::regular);
  volInt::polyhedron cur_main_bound_model =
    read_obj_prefix("main_bound", c3d::c3d_type::bound);

  // Must be called before call to remove_polygons() and get_m3d_header_data()
  read_file_cfg_m3d(cur_main_model);



  // Must be called before call to get_m3d_scale_size()
  remove_polygons(cur_main_model, remove_polygons_model::regular);


  get_m3d_scale_size(&cur_main_model, &cur_main_bound_model);

  get_m3d_header_data(&cur_main_model, &cur_main_bound_model);



  std::size_t m3d_file_size =
    get_m3d_file_size(&cur_main_model, &cur_main_bound_model);



  // Allocating enough space for *.m3d file
  m3d_data = std::string(m3d_file_size, '\0');
  m3d_data_cur_pos = 0;

  write_c3d(cur_main_model);
  write_m3d_header_data();
  write_c3d(cur_main_bound_model);
  write_var_to_m3d<int>(weapon_slots_existence);



  boost::filesystem::path file_to_save = output_dir_path;
  file_to_save.append(model_name + ".m3d", boost::filesystem::path::codecvt());
  helpers::save_file(file_to_save,
                     m3d_data,
                     TRACTOR_CONVERTER_BINARY,
                     output_file_name_error);



  // TEST
  /*
  std::cout << "second time!" << '\n';
  std::cout << "model:" << model_name << '\n';
  std::cout << "scale_size: " << scale_size << '\n';

  volInt::vector_multiply_self(cur_main_model.rcm, scale_size);

  for(auto &&vert : cur_main_model.verts)
  {
    volInt::vector_multiply_self(vert, scale_size);
  }
//std::cout << model_name << " main mechos models after scaling." << '\n';
  cur_main_model.faces_calc_params();
  cur_main_model.calculate_c3d_properties();
  */



  // TEST
//std::cout << '\n';
//std::cout << "scale_size of model: " <<
//  input_file_path << " is " << scale_size << '\n';
}



void wavefront_obj_to_m3d_model::read_file_cfg_helper_overwrite_volume(
  volInt::polyhedron &model,
  const double custom_volume)
{
  model.volume_overwritten = true;
  model.volume = custom_volume;

  // TEST
//  std::cout << "read_file_cfg_helper_overwrite_volume:" <<
//    model.volume << '\n';
}



void wavefront_obj_to_m3d_model::read_file_cfg_helper_overwrite_J(
  volInt::polyhedron &model,
  const std::vector<double> &custom_J)
{
  model.J_overwritten = true;
  std::size_t cur_custom_J_el = 0;
  for(std::size_t cur_row = 0; cur_row < 3; ++cur_row)
  {
    for(std::size_t cur_el = 0; cur_el < 3; ++cur_el)
    {
      model.J[cur_row][cur_el] = custom_J[cur_custom_J_el];
      ++cur_custom_J_el;
    }
  }

  // TEST
  /*
  std::cout << "read_file_cfg_helper_overwrite_J:" << '\n';
  for(std::size_t cur_row = 0; cur_row < 3; ++cur_row)
  {
    for(std::size_t cur_el = 0; cur_el < 3; ++cur_el)
    {
      std::cout << "model.J[" << cur_row << "][" << cur_el << "]" <<
        model.J[cur_row][cur_el] << '\n';
    }
  }
  */
}



void wavefront_obj_to_m3d_model::read_file_cfg_m3d(
  volInt::polyhedron &main_model,
  std::deque<volInt::polyhedron> *debris_models)
{
  boost::program_options::variables_map vm;

  try
  {
    boost::filesystem::path config_file_path =
      input_file_path / (model_name + ".cfg");
    std::string config_file_str = config_file_path.string();

    // Declare a group of options that will be
    // allowed in config file.
    boost::program_options::options_description config("per_file_cfg");
    config.add_options()
      ("overwrite_volume_main",
       boost::program_options::bool_switch()->default_value(false),
       "Overwrite volume for main model when custom volume is supplied.")
      ("custom_center_of_mass_main",
       boost::program_options::bool_switch()->default_value(false),
       "Use center of mass mark to get custom center of mass for main model.")
      ("overwrite_inertia_tensor_main",
       boost::program_options::bool_switch()->default_value(false),
       "Overwrite inertia tensor matrix for main model "
         "when custom matrix is supplied.")
      ("custom_volume_main",
       boost::program_options::value<std::string>(),
       "Custom volume for main model.")
      ("custom_inertia_tensor_main",
       boost::program_options::value<std::vector<std::string>>(),
       "Custom inertia tensor matrix for main model.")
    ;

    if(debris_models && debris_models->size())
    {
      config.add_options()
        ("overwrite_volume_debris",
         boost::program_options::bool_switch()->default_value(false),
         "Overwrite volume for debris models when custom volume is supplied.")
        ("custom_center_of_mass_debris",
         boost::program_options::bool_switch()->default_value(false),
         "Use center of mass mark to get custom center of mass "
           "for debris models.")
        ("overwrite_inertia_tensor_debris",
         boost::program_options::bool_switch()->default_value(false),
         "Overwrite inertia tensor matrix for debris models "
           "when custom matrix is supplied.")
      ;
      for(std::size_t cur_debris = 0,
            debris_models_size = debris_models->size();
          cur_debris < debris_models_size;
          ++cur_debris)
      {
        config.add_options()
          (("custom_volume_debris_" + std::to_string(cur_debris + 1)).c_str(),
           boost::program_options::value<std::string>(),
           ("Custom volume for debris model " +
              std::to_string(cur_debris + 1) + ".").c_str())
          (("custom_inertia_tensor_debris_" + std::to_string(cur_debris + 1)).
             c_str(),
           boost::program_options::value<std::vector<std::string>>(),
           ("Custom inertia tensor matrix for debris model " +
              std::to_string(cur_debris + 1) + ".").c_str())
        ;
      }
    }



    std::ifstream ifs(config_file_str.c_str());
    if(ifs)
    {
      boost::program_options::store(parse_config_file(ifs, config), vm);
      boost::program_options::notify(vm);
    }
    else
    {
//    std::cout << "Can not open config file: " << config_file_str << "\n";
      throw std::runtime_error(
        "Can not open config file: \"" + config_file_str + "\".");
    }



    if(vm["overwrite_volume_main"].as<bool>())
    {
      const double custom_volume_main =
        parse_per_file_cfg_option<double>(
          vm["custom_volume_main"].as<std::string>());
      read_file_cfg_helper_overwrite_volume(main_model, custom_volume_main);
    }

    if(vm["custom_center_of_mass_main"].as<bool>())
    {
      try
      {
        get_custom_rcm(main_model);
      }
      catch(std::exception &e)
      {
        std::cout << "Failed to retrieve custom center of mass in " <<
          input_file_name_error << " file " <<
          main_model.wavefront_obj_path << '\n';
        std::cout << e.what();
        std::cout <<
          "Center of mass is not overwritten for main model." << '\n';
      }
    }

    if(vm["overwrite_inertia_tensor_main"].as<bool>())
    {
      if(vm.count("custom_inertia_tensor_main"))
      {
        // 9 doubles to describe 3x3 inertia tensor matrix
        const std::vector<double> custom_J_main =
          parse_per_file_cfg_multiple_options<double>(
            vm["custom_inertia_tensor_main"].as<std::vector<std::string>>());
        if(custom_J_main.size() == J_cfg_num_of_values)
        {
          read_file_cfg_helper_overwrite_J(main_model, custom_J_main);
        }
        else
        {
          std::cout <<
            "In config file " << config_file_str <<
            " unexpected number of values specified " <<
            "for custom_inertia_tensor_main" << '\n';
          std::cout <<
            "Expected " << std::to_string(J_cfg_num_of_values) <<
            ", got  " << custom_J_main.size() << '\n';
          std::cout <<
            "Inertia tensor is not overwritten for main model." << '\n';
        }
      }
    }


    if(debris_models && debris_models->size())
    {
      if(vm["overwrite_volume_debris"].as<bool>())
      {
        for(std::size_t cur_debris = 0,
              debris_models_size = debris_models->size();
            cur_debris < debris_models_size;
            ++cur_debris)
        {
          std::string cur_custom_volume_option =
            "custom_volume_debris_" + std::to_string(cur_debris + 1);
          if(vm.count(cur_custom_volume_option))
          {
            const double custom_volume_debris =
              parse_per_file_cfg_option<double>(
                vm[cur_custom_volume_option].as<std::string>());
            read_file_cfg_helper_overwrite_volume((*debris_models)[cur_debris],
                                                  custom_volume_debris);
          }
        }
      }

      if(vm["custom_center_of_mass_debris"].as<bool>())
      {
        for(std::size_t cur_debris = 0,
              debris_models_size = debris_models->size();
            cur_debris < debris_models_size;
            ++cur_debris)
        {
          try
          {
            get_custom_rcm((*debris_models)[cur_debris]);
          }
          catch(std::exception &e)
          {
            std::cout <<
              "Failed to retrieve custom center of mass in " <<
              input_file_name_error << " file " <<
              (*debris_models)[cur_debris].wavefront_obj_path << '\n';
            std::cout <<
              "Center of mass is not overwritten for debris model " <<
              std::to_string(cur_debris + 1) << "." << '\n';
          }
        }
      }

      if(vm["overwrite_inertia_tensor_debris"].as<bool>())
      {
        for(std::size_t cur_debris = 0,
              debris_models_size = debris_models->size();
            cur_debris < debris_models_size;
            ++cur_debris)
        {
          std::string cur_custom_J_option =
            "custom_inertia_tensor_debris_" + std::to_string(cur_debris + 1);
          if(vm.count(cur_custom_J_option))
          {
            // 9 doubles to describe 3x3 inertia tensor matrix
            const std::vector<double> custom_J_debris =
              parse_per_file_cfg_multiple_options<double>(
                vm[cur_custom_J_option].as<std::vector<std::string>>());
            if(custom_J_debris.size() == J_cfg_num_of_values)
            {
              read_file_cfg_helper_overwrite_J(
                (*debris_models)[cur_debris],
                custom_J_debris);
            }
            else
            {
              std::cout <<
                "In config file " << config_file_str <<
                " unexpected number of values specified " <<
                "for " << cur_custom_J_option << '\n';
              std::cout <<
                "Expected " << std::to_string(J_cfg_num_of_values) <<
                ", got  " << custom_J_debris.size() << '\n';
              std::cout <<
                "Inertia tensor is not overwritten for debris " <<
                std::to_string(cur_debris + 1) << "." << '\n';
            }
          }
        }
      }
    }
  }
  catch(std::exception &)
  {
    std::cout <<
      "Failed to get config options for model " << model_name << '\n';
    throw;
  }
}



void wavefront_obj_to_m3d_model::read_file_cfg_a3d(
  std::deque<volInt::polyhedron> &animated_models)
{
  boost::program_options::variables_map vm;

  try
  {
    boost::filesystem::path config_file_path =
      input_file_path / (model_name + ".cfg");
    std::string config_file_str = config_file_path.string();

    // Declare a group of options that will be
    // allowed in config file.
    boost::program_options::options_description config("per_file_cfg");
    config.add_options()
      ("overwrite_volume_animated",
       boost::program_options::bool_switch()->default_value(false),
       "Overwrite volume for animated models when custom volume is supplied.")
      ("custom_center_of_mass_animated",
       boost::program_options::bool_switch()->default_value(false),
       "Use center of mass mark to get custom center of mass "
         "for animated models.")
      ("overwrite_inertia_tensor_animated",
       boost::program_options::bool_switch()->default_value(false),
       "Overwrite inertia tensor matrix for animated models "
         "when custom matrix is supplied.")
    ;

    for(std::size_t cur_animated = 0,
          animated_models_size = animated_models.size();
        cur_animated < animated_models_size;
        ++cur_animated)
    {
      config.add_options()
        (("custom_volume_animated_" +
            std::to_string(cur_animated + 1)).c_str(),
         boost::program_options::value<std::string>(),
         ("Custom volume for animated model " +
            std::to_string(cur_animated + 1) + ".").c_str())
        (("custom_inertia_tensor_animated_" +
            std::to_string(cur_animated + 1)).c_str(),
         boost::program_options::value<std::vector<std::string>>(),
         ("Custom inertia tensor matrix for animated model " +
            std::to_string(cur_animated + 1) + ".").c_str())
      ;
    }



    std::ifstream ifs(config_file_str.c_str());
    if(ifs)
    {
      boost::program_options::store(parse_config_file(ifs, config), vm);
      boost::program_options::notify(vm);
    }
    else
    {
//    std::cout << "Can not open config file: " << config_file_str << "\n";
      throw std::runtime_error(
        "Can not open config file: \"" + config_file_str + "\".");
    }



    if(vm["overwrite_volume_animated"].as<bool>())
    {
      for(std::size_t cur_animated = 0,
            animated_models_size = animated_models.size();
          cur_animated < animated_models_size;
          ++cur_animated)
      {
        std::string cur_custom_volume_option =
          "custom_volume_animated_" + std::to_string(cur_animated + 1);
        if(vm.count(cur_custom_volume_option))
        {
          const double custom_volume_animated =
            parse_per_file_cfg_option<double>(
              vm[cur_custom_volume_option].as<std::string>());
          read_file_cfg_helper_overwrite_volume(animated_models[cur_animated],
                                                custom_volume_animated);
        }
      }
    }

    if(vm["custom_center_of_mass_animated"].as<bool>())
    {
      for(std::size_t cur_animated = 0,
            animated_models_size = animated_models.size();
          cur_animated < animated_models_size;
          ++cur_animated)
      {
        try
        {
          get_custom_rcm(animated_models[cur_animated]);
        }
        catch(std::exception &e)
        {
          std::cout <<
            "Failed to retrieve custom center of mass in " <<
            input_file_name_error << " file " <<
            animated_models[cur_animated].wavefront_obj_path << '\n';
          std::cout <<
            "Center of mass is not overwritten for animated model " <<
            std::to_string(cur_animated + 1) << "." << '\n';
        }
      }
    }

    if(vm["overwrite_inertia_tensor_animated"].as<bool>())
    {
      for(std::size_t cur_animated = 0,
            animated_models_size = animated_models.size();
          cur_animated < animated_models_size;
          ++cur_animated)
      {
        std::string cur_custom_J_option =
          "custom_inertia_tensor_animated_" + std::to_string(cur_animated + 1);
        if(vm.count(cur_custom_J_option))
        {
          // 9 doubles to describe 3x3 inertia tensor matrix
          const std::vector<double> custom_J_animated =
            parse_per_file_cfg_multiple_options<double>(
              vm[cur_custom_J_option].as<std::vector<std::string>>());
          if(custom_J_animated.size() == J_cfg_num_of_values)
          {
            read_file_cfg_helper_overwrite_J(
              animated_models[cur_animated],
              custom_J_animated);
          }
          else
          {
            std::cout <<
              "In config file " << config_file_str <<
              " unexpected number of values specified " <<
              "for " << cur_custom_J_option << '\n';
            std::cout <<
              "Expected " << std::to_string(J_cfg_num_of_values) <<
              ", got  " << custom_J_animated.size() << '\n';
            std::cout <<
              "Inertia tensor is not overwritten for animated model " <<
              std::to_string(cur_animated + 1) << "." << '\n';
          }
        }
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << "Failed to get config options for model " <<
      model_name << '\n';
    throw;
  }
}



volInt::polyhedron wavefront_obj_to_m3d_model::read_obj(
  const boost::filesystem::path &obj_input_file_path,
  c3d::c3d_type cur_c3d_type)
{
  return raw_obj_to_volInt_model(obj_input_file_path,
                                 input_file_name_error,
                                 cur_c3d_type,
                                 c3d_default_material_id);
}



volInt::polyhedron wavefront_obj_to_m3d_model::read_obj_prefix(
  const std::string &filename_prefix,
  c3d::c3d_type cur_c3d_type)
{
  boost::filesystem::path obj_input_file_path =
    input_file_path /
    (input_file_path.filename().string() + "_" + filename_prefix + ".obj");

  return read_obj(obj_input_file_path, cur_c3d_type);
}



std::deque<volInt::polyhedron>
  wavefront_obj_to_m3d_model::read_objs_with_prefix(
    const std::string &prefix,
    c3d::c3d_type cur_c3d_type)
{
  std::deque<volInt::polyhedron> models_to_return;
  std::string full_prefix;
  if(prefix.empty())
  {
    full_prefix = model_name + "_";
  }
  else
  {
    full_prefix = model_name + "_" + prefix + "_";
  }

  std::size_t to_reserve = 0;

  for(std::size_t cur_model = 0; ; ++cur_model)
  {
    boost::filesystem::path cur_path =
      input_file_path /
      (full_prefix + std::to_string(cur_model + 1) + ".obj");
    try
    {
      models_to_return.push_back(read_obj(cur_path, cur_c3d_type));
    }
    // Expected. Stopping the loop when next file in the list is not found. 
    catch(exception::file_not_found &)
    {
      break;
    }
  }

  return models_to_return;
}





std::vector<double> wavefront_obj_to_m3d_model::get_medium_vert(
  const volInt::polyhedron &model,
  std::size_t poly_n)
{
  std::vector<double> medium_vert(3, 0.0);
  // For polygon with zero_reserved color middle point is different.
  // middle_x is either xmax of M3D or -xmax of M3D.
  // middle_y is either ymax of M3D or -ymax of M3D.
  // middle_z for all those polygons is zmin of bound C3D.
  // In all other polygons middle point is average vertex.
  if(model.faces[poly_n].color_id ==
     c3d::color::string_to_id::zero_reserved)
  {
    // preserved sign
    std::vector<double> extreme_abs_coords(3, 0.0);
    for(const auto vert_ind : model.faces[poly_n].verts)
    {
      const std::vector<double> &vert = model.verts[vert_ind];
      for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
      {
        if(std::abs(vert[cur_coord]) >
           std::abs(extreme_abs_coords[cur_coord]))
        {
          extreme_abs_coords[cur_coord] = vert[cur_coord];
        }
      }
    }
    if(std::signbit(extreme_abs_coords[0]))
    {
      medium_vert[0] = -xmax();
    }
    else
    {
      medium_vert[0] = xmax();
    }
    if(std::signbit(extreme_abs_coords[1]))
    {
      medium_vert[1] = -ymax();
    }
    else
    {
      medium_vert[1] = ymax();
    }
    medium_vert[2] = model.zmin();
    // TEST
    /*
    if(model_name == "m4")
    {
      std::cout << "\n" << '\n';
      std::cout << "m4: " << '\n';
      std::cout << "poly_n: " << poly_n << '\n';
      std::size_t cur_vert = 0;
      for(const auto vert_ind : model.faces[poly_n].verts)
      {
        const std::vector<double> &vert = model.verts[vert_ind];
        std::cout << "vert " << cur_vert;
        for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
        {
          std::cout << " " << vert[cur_coord];
        }
        std::cout << "\n";
        ++cur_vert;
      }
      std::cout << "medium_vert: ";
      for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
      {
        std::cout << " " << medium_vert[cur_coord];
      }
      std::cout << "\n";
      std::cout << "-----------------------";
    }
    */
  }
  else
  {
    for(int vert_n = 0; vert_n < model.faces[poly_n].numVerts; ++vert_n)
    {
      volInt::vector_plus_self(medium_vert,
                               model.verts[model.faces[poly_n].verts[vert_n]]);
    }
    volInt::vector_divide_self(medium_vert, model.faces[poly_n].numVerts);
  }
  return medium_vert;
}



void wavefront_obj_to_m3d_model::write_c3d(const volInt::polyhedron &model)
{
  write_var_to_m3d<int>(c3d::version_req);

  write_var_to_m3d<int>(model.numVerts);
  write_var_to_m3d<int>(model.numVertNorms);
  write_var_to_m3d<int>(model.numFaces);
  write_var_to_m3d<int>(model.numVertTotal);

  write_var_to_m3d<int>(
    round_half_to_even<double, int>(model.xmax() * scale_size));
  write_var_to_m3d<int>(
    round_half_to_even<double, int>(model.ymax() * scale_size));
  write_var_to_m3d<int>(
    round_half_to_even<double, int>(model.zmax() * scale_size));

  write_var_to_m3d<int>(
    round_half_to_even<double, int>(model.xmin() * scale_size));
  write_var_to_m3d<int>(
    round_half_to_even<double, int>(model.ymin() * scale_size));
  write_var_to_m3d<int>(
    round_half_to_even<double, int>(model.zmin() * scale_size));

  write_var_to_m3d<int>(
    round_half_to_even<double, int>(model.x_off() * scale_size));
  write_var_to_m3d<int>(
    round_half_to_even<double, int>(model.y_off() * scale_size));
  write_var_to_m3d<int>(
    round_half_to_even<double, int>(model.z_off() * scale_size));

  write_var_to_m3d<int>(
    round_half_to_even<double, int>(model.rmax * scale_size));

  write_var_to_m3d<int>(c3d::default_phi_psi_tetta);
  write_var_to_m3d<int>(c3d::default_phi_psi_tetta);
  write_var_to_m3d<int>(c3d::default_phi_psi_tetta);

  write_var_to_m3d<double>(model.volume * std::pow(scale_size, 3));

  for(auto rcm_coord : model.rcm)
  {
    write_var_to_m3d<double>(rcm_coord * scale_size);
  }

  std::vector<std::vector<double>> scaled_J = model.J;
  double J_scale = std::pow(scale_size, 5);
  for(auto &&scaled_J_row : scaled_J)
  {
    for(auto &&scaled_J_el : scaled_J_row)
    {
      scaled_J_el *= J_scale;
    }
  }

  write_nest_vec_var_to_m3d<double>(scaled_J);
//write_nest_vec_var_to_m3d<double>(model.J);

  for(int cur_vert = 0; cur_vert < model.numVerts; ++cur_vert)
  {
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      write_var_to_m3d<float>(model.verts[cur_vert][cur_coord] * scale_size);
    }
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      write_var_to_m3d<char>(
        round_half_to_even<double, char>(
          model.verts[cur_vert][cur_coord] * scale_size));
    }
    write_var_to_m3d<int>(c3d::vertex::default_sort_info);
  }

  for(int cur_norm = 0; cur_norm < model.numVertNorms; ++cur_norm)
  {
    std::vector<double> scaled =
      volInt::vector_scale(c3d::vector_scale_val,
                           model.vertNorms[cur_norm]);
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      write_var_to_m3d<char>(
        round_half_to_even<double, char>(scaled[cur_coord]));
    }
    write_var_to_m3d<unsigned char>(c3d::normal::default_n_power);
    write_var_to_m3d<int>(c3d::normal::default_sort_info);
  }

  for(int cur_poly_n = 0; cur_poly_n < model.numFaces; ++cur_poly_n)
  {
    write_var_to_m3d<int>(model.faces[cur_poly_n].numVerts);
    write_var_to_m3d<int>(c3d::polygon::default_sort_info);
    write_var_to_m3d<unsigned int>(model.faces[cur_poly_n].color_id);

    // TEST
    /*
    if(model_name == "m4")
    {
      std::cout << "m4: " << '\n';
      std::cout << "cur_poly_n: " << cur_poly_n << '\n';
      std::cout << "color_id: " << model.faces[cur_poly_n].color_id << '\n';
      std::cout << "color_shift: " <<
        c3d::color::id_to_shift[model.faces[cur_poly_n].color_id] << '\n';
    }
    */

    /*
    if(model.faces[cur_poly_n].color_id == c3d::color::string_to_id::body)
    {
      write_var_to_m3d<unsigned int>(body_color_shift);
    }
    else
    {
      write_var_to_m3d<unsigned int>(
        c3d::color::id_to_shift[model.faces[cur_poly_n].color_id]);
    }
    */
    // color_shift is always 0
    write_var_to_m3d<unsigned int>(c3d::polygon::default_color_shift);

    std::vector<double> scaled =
      volInt::vector_scale(c3d::vector_scale_val,
                           model.faces[cur_poly_n].norm);
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      write_var_to_m3d<char>(
        round_half_to_even<double, char>(scaled[cur_coord]));
    }
    write_var_to_m3d<unsigned char>(c3d::polygon::default_flat_norm_n_power);

    std::vector<double> medium_vert = get_medium_vert(model, cur_poly_n);
    std::vector<char> medium_vert_char(3);
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      medium_vert_char[cur_coord] =
        round_half_to_even<double, char>(medium_vert[cur_coord] * scale_size);
    }
    write_vec_var_to_m3d<char>(medium_vert_char);

//  for(std::size_t vert_n = 0;
//      vert_n < model.faces[cur_poly_n].numVerts;
//      ++vert_n)
    // Note the reverse order of vertices.
    for(int vert_n = model.faces[cur_poly_n].numVerts - 1;
        vert_n != -1;
        --vert_n)
    {
      write_var_to_m3d<int>(model.faces[cur_poly_n].verts[vert_n]);
      write_var_to_m3d<int>(model.faces[cur_poly_n].vertNorms[vert_n]);
    }
  }

  std::size_t skipped_sorted_bytes_n =
    model.numFaces * c3d::polygon_sort_info::size;
  std::memset(&m3d_data[m3d_data_cur_pos], 0, skipped_sorted_bytes_n);
  m3d_data_cur_pos += skipped_sorted_bytes_n;
}



void wavefront_obj_to_m3d_model::write_m3d_header_data()
{
  write_var_to_m3d<int>(round_half_to_even<double, int>(xmax() * scale_size));
  write_var_to_m3d<int>(round_half_to_even<double, int>(ymax() * scale_size));
  write_var_to_m3d<int>(round_half_to_even<double, int>(zmax() * scale_size));
  write_var_to_m3d<int>(round_half_to_even<double, int>(rmax * scale_size));

  write_var_to_m3d<int>(n_wheels);
  write_var_to_m3d<int>(n_debris);

  write_var_to_m3d<int>(body_color_offset);
  write_var_to_m3d<int>(body_color_shift);
}



void wavefront_obj_to_m3d_model::write_a3d_header_data()
{
  write_var_to_m3d<int>(n_models);

  write_var_to_m3d<int>(round_half_to_even<double, int>(xmax() * scale_size));
  write_var_to_m3d<int>(round_half_to_even<double, int>(ymax() * scale_size));
  write_var_to_m3d<int>(round_half_to_even<double, int>(zmax() * scale_size));
  write_var_to_m3d<int>(round_half_to_even<double, int>(rmax * scale_size));

  write_var_to_m3d<int>(body_color_offset);
  write_var_to_m3d<int>(body_color_shift);
}



void wavefront_obj_to_m3d_model::write_m3d_wheels_data(
  std::unordered_map<int, volInt::polyhedron> &wheels_models)
{
  for(int i = 0; i < n_wheels; i++)
  {
    write_var_to_m3d<int>(cur_wheel_data[i].steer);
    volInt::vector_multiply_self(cur_wheel_data[i].r, scale_size);
    write_vec_var_to_m3d<double>(cur_wheel_data[i].r);
    write_var_to_m3d<int>(cur_wheel_data[i].width * scale_size);
    write_var_to_m3d<int>(cur_wheel_data[i].radius * scale_size);
    write_var_to_m3d<int>(cur_wheel_data[i].bound_index);
    if(cur_wheel_data[i].steer)
    {
      write_c3d(wheels_models[i]);
    }
  }
}



void wavefront_obj_to_m3d_model::write_m3d_debris_data(
  std::deque<volInt::polyhedron> &debris_models,
  std::deque<volInt::polyhedron> &debris_bound_models)
{
  for(int i = 0; i < n_debris; i++)
  {
    write_c3d(debris_models[i]);
    write_c3d(debris_bound_models[i]);
  }
}



void wavefront_obj_to_m3d_model::write_m3d_weapon_slots()
{
  for(std::size_t i = 0; i < m3d::weapon_slot::max_slots; i++)
  {

    // TEST
    /*
    std::cout <<
      "cur_weapon_slot_data[" << i << "].R_slots[0]" <<
      cur_weapon_slot_data[i].R_slots[0] << '\n';
    std::cout <<
      "cur_weapon_slot_data[" << i << "].R_slots[1]" <<
      cur_weapon_slot_data[i].R_slots[1] << '\n';
    std::cout <<
      "cur_weapon_slot_data[" << i << "].R_slots[2]" <<
      cur_weapon_slot_data[i].R_slots[2] << '\n';
    std::cout <<
      "cur_weapon_slot_data[" << i << "].location_angle_of_slots:" <<
      cur_weapon_slot_data[i].location_angle_of_slots << '\n';
    std::cout <<
      "cur_weapon_slot_data[" << i << "].location_angle_of_slots:" <<
      radians_to_sicher_angle(
        cur_weapon_slot_data[i].location_angle_of_slots) <<
      '\n';
    */

    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      write_var_to_m3d<int>(
        round_half_to_even<double, int>(
          cur_weapon_slot_data[i].R_slots[cur_coord] * scale_size));
    }
    write_var_to_m3d<int>(
      radians_to_sicher_angle(
        cur_weapon_slot_data[i].location_angle_of_slots));
  }
}





std::vector<point*>
  wavefront_obj_to_m3d_model::get_ref_points_for_part_of_model(
    volInt::polyhedron &model,
    const volInt::polyhedron *reference_model,
    unsigned int color_id,
    int wheel_weapon_id)
{
  std::vector<point*> ref_points(3, nullptr);

  std::pair<int, int> vert_indices(0, 0);

  for(int cur_poly_num = 0; cur_poly_num < model.numFaces; ++cur_poly_num)
  {
    volInt::face &cur_poly = model.faces[cur_poly_num];
    if(cur_poly.color_id == color_id &&
       (wheel_weapon_id < 0 || cur_poly.wheel_weapon_id == wheel_weapon_id))
    {
      for(int cur_vert_num = 0;
          cur_vert_num < model.numVertsPerPoly;
          ++cur_vert_num)
      {
        // ref_vert_one_ind is std::pair<int, int>
        // first is position of polygon which contains vertex of reference.
        // second is vertex index in this polygon.
        if(reference_model->ref_vert_one_ind == vert_indices)
        {
          ref_points[0] =
            &model.verts[model.faces[cur_poly_num].verts[cur_vert_num]];
        }
        else if(reference_model->ref_vert_two_ind == vert_indices)
        {
          ref_points[1] =
            &model.verts[model.faces[cur_poly_num].verts[cur_vert_num]];
        }
        else if(reference_model->ref_vert_three_ind == vert_indices)
        {
          ref_points[2] =
            &model.verts[model.faces[cur_poly_num].verts[cur_vert_num]];
        }
        ++vert_indices.second;
      }
      ++vert_indices.first;
      vert_indices.second = 0;
    }
  }


  // TEST
  /*
  std::cout << "\n\n\n";
  std::cout << "reference_model->ref_vert_one_ind: " <<
    reference_model->ref_vert_one_ind.first << "; " <<
    reference_model->ref_vert_one_ind.second << '\n';
  std::cout << "reference_model->ref_vert_two_ind: " <<
    reference_model->ref_vert_two_ind.first << "; " <<
    reference_model->ref_vert_two_ind.second << '\n';
  std::cout << "reference_model->ref_vert_three_ind: " <<
    reference_model->ref_vert_three_ind.first << "; " <<
    reference_model->ref_vert_three_ind.second << '\n';
  std::cout << "ref_points[0]: " <<
    reinterpret_cast<void*>(ref_points[0]) << '\n';
  std::cout << "ref_points[1]: " <<
    reinterpret_cast<void*>(ref_points[1]) << '\n';
  std::cout << "ref_points[2]: " <<
    reinterpret_cast<void*>(ref_points[2]) << '\n';
  */

  if(!(ref_points[0] &&
       ref_points[1] &&
       ref_points[2]))
  {
    std::string err_msg;
    err_msg.append(
      input_file_name_error + " file " + model.wavefront_obj_path + "\n" +
      "Can't find reference points for " +
      c3d::color::ids.by<c3d::color::id>().at(color_id));
    if(wheel_weapon_id >= 0)
    {
      err_msg.append(" " + std::to_string(wheel_weapon_id + 1));
    }
    err_msg.append(".\n");
    throw exception::model_ref_points_not_found(err_msg);
  }

  return ref_points;
}



std::pair<point, point> wavefront_obj_to_m3d_model::get_compare_points(
  std::vector<point*> cur_ref_verts,
  const volInt::polyhedron *reference_model)
{

  point two_rel_to_one = volInt::vector_minus((*cur_ref_verts[1]),
                                              (*cur_ref_verts[0]));

  point three_rel_to_one = volInt::vector_minus((*cur_ref_verts[2]),
                                                (*cur_ref_verts[0]));

  point two_compare_point =
    volInt::vector_minus(two_rel_to_one,
                         reference_model->ref_vert_two_rel_to_one);
  point three_compare_point =
    volInt::vector_minus(three_rel_to_one,
                         reference_model->ref_vert_three_rel_to_one);

  // TEST
  /*
  std::cout << "two_rel_to_one: " <<
    two_rel_to_one[0] << "; " <<
    two_rel_to_one[1] << ";" <<
    two_rel_to_one[2] << '\n';

  std::cout << "reference_model->ref_vert_two_rel_to_one: " <<
    reference_model->ref_vert_two_rel_to_one[0] << "; " <<
    reference_model->ref_vert_two_rel_to_one[1] << ";" <<
    reference_model->ref_vert_two_rel_to_one[2] << '\n';

  std::cout << "two_compare_point: " <<
    two_compare_point[0] << "; " <<
    two_compare_point[1] << ";" <<
    two_compare_point[2] << '\n';

  std::cout << "three_rel_to_one: " <<
    three_rel_to_one[0] << "; " <<
    three_rel_to_one[1] << ";" <<
    three_rel_to_one[2] << '\n';

  std::cout << "reference_model->ref_vert_three_rel_to_one: " <<
    reference_model->ref_vert_three_rel_to_one[0] << "; " <<
    reference_model->ref_vert_three_rel_to_one[1] << ";" <<
    reference_model->ref_vert_three_rel_to_one[2] << '\n';

  std::cout << "three_compare_point: " <<
    three_compare_point[0] << "; " <<
    three_compare_point[1] << ";" <<
    three_compare_point[2] << '\n';
  */

  return {two_compare_point, three_compare_point};
}



std::pair<point, point> wavefront_obj_to_m3d_model::get_compare_points(
  std::vector<point*> cur_ref_verts,
  std::vector<point*> ref_model_ref_verts)
{

  point two_rel_to_one =
    volInt::vector_minus((*cur_ref_verts[1]), (*cur_ref_verts[0]));

  point three_rel_to_one =
    volInt::vector_minus((*cur_ref_verts[2]), (*cur_ref_verts[0]));

  point ref_model_two_rel_to_one =
    volInt::vector_minus((*ref_model_ref_verts[1]), (*ref_model_ref_verts[0]));

  point ref_model_three_rel_to_one =
    volInt::vector_minus((*ref_model_ref_verts[2]), (*ref_model_ref_verts[0]));

  point two_compare_point =
    volInt::vector_minus(two_rel_to_one, ref_model_two_rel_to_one);
  point three_compare_point =
    volInt::vector_minus(three_rel_to_one, ref_model_three_rel_to_one);

  // TEST
  /*
  std::cout << "two_rel_to_one: " <<
    two_rel_to_one[0] << "; " <<
    two_rel_to_one[1] << ";" <<
    two_rel_to_one[2] << '\n';

  std::cout << "reference_model->ref_vert_two_rel_to_one: " <<
    reference_model->ref_vert_two_rel_to_one[0] << "; " <<
    reference_model->ref_vert_two_rel_to_one[1] << ";" <<
    reference_model->ref_vert_two_rel_to_one[2] << '\n';

  std::cout << "two_compare_point: " <<
    two_compare_point[0] << "; " <<
    two_compare_point[1] << ";" <<
    two_compare_point[2] << '\n';

  std::cout << "three_rel_to_one: " <<
    three_rel_to_one[0] << "; " <<
    three_rel_to_one[1] << ";" <<
    three_rel_to_one[2] << '\n';

  std::cout << "reference_model->ref_vert_three_rel_to_one: " <<
    reference_model->ref_vert_three_rel_to_one[0] << "; " <<
    reference_model->ref_vert_three_rel_to_one[1] << ";" <<
    reference_model->ref_vert_three_rel_to_one[2] << '\n';

  std::cout << "three_compare_point: " <<
    three_compare_point[0] << "; " <<
    three_compare_point[1] << ";" <<
    three_compare_point[2] << '\n';
  */

  return {two_compare_point, three_compare_point};
}



// rcm - center of mass
void wavefront_obj_to_m3d_model::get_custom_rcm(volInt::polyhedron &model)
{
  if(!center_of_mass_model)
  {
    return;
  }

  std::vector<point*> cur_rcm_verts =
    get_ref_points_for_part_of_model(
      model,
      center_of_mass_model,
      c3d::color::string_to_id::center_of_mass);

  std::pair<point, point> compare_points =
    get_compare_points(
      cur_rcm_verts,
      center_of_mass_model);



  // If all coordinates of points 2 and 3 relative to 1 are the same
  // in orig model and current model then model wasn't rotated or changed.
  // compare_points.first is difference between 2nd reference points.
  // compare_points.second is difference between 3rd ones.
  // Throwing exception if difference is not zero.
  for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
  {
    if(!(std::abs(compare_points.first[cur_coord]) <
         volInt::ref_points_distinct_distance &&
       std::abs(compare_points.second[cur_coord]) <
         volInt::ref_points_distinct_distance))
    {
      throw std::runtime_error(
        input_file_name_error + " file " +
        model.wavefront_obj_path + '\n' +
        "Center of mass was rotated, scaled or changed." + '\n' +
        "Only moving is allowed for center of mass." + '\n');
    }
  }

  model.rcm = volInt::vector_minus(*cur_rcm_verts[0],
                                   *center_of_mass_model->ref_vert_one);

  model.rcm_overwritten = true;

  // TEST
  /*
  std::cout << "*cur_rcm_verts[0]: " <<
    (*cur_rcm_verts[0])[0] << "; " <<
    (*cur_rcm_verts[0])[1] << "; " <<
    (*cur_rcm_verts[0])[2] << '\n';
  std::cout << "*center_of_mass_model->ref_vert_one: " <<
    (*center_of_mass_model->ref_vert_one)[0] << "; " <<
    (*center_of_mass_model->ref_vert_one)[1] << "; " <<
    (*center_of_mass_model->ref_vert_one)[2] << '\n';
  */
}



void wavefront_obj_to_m3d_model::get_attachment_point(
  volInt::polyhedron &model)
{
  if(!weapon_attachment_point)
  {
    return;
  }

  std::vector<point*> cur_attachment_verts =
    get_ref_points_for_part_of_model(
      model,
      weapon_attachment_point,
      c3d::color::string_to_id::attachment_point);

  std::pair<point, point> compare_points =
    get_compare_points(
      cur_attachment_verts,
      weapon_attachment_point);



  // If all coordinates of points 2 and 3 relative to 1 are the same
  // in orig model and current model then model wasn't rotated or changed.
  // compare_points.first is difference between 2nd reference points.
  // compare_points.second is difference between 3rd ones.
  // Throwing exception if difference is not zero.
  for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
  {
    if(!(std::abs(compare_points.first[cur_coord]) <
         volInt::ref_points_distinct_distance &&
       std::abs(compare_points.second[cur_coord]) <
         volInt::ref_points_distinct_distance))
    {
      throw std::runtime_error(
        input_file_name_error + " file " +
        model.wavefront_obj_path + '\n' +
        "Attachment point was rotated, scaled or changed." + '\n' +
        "Only moving is allowed for attachment point in weapon model." + '\n');
    }
  }

  model.offset_point() =
    volInt::vector_minus(*cur_attachment_verts[0],
                         *weapon_attachment_point->ref_vert_one);
}



// Using weapon attachment point model to get position of weapons.
void wavefront_obj_to_m3d_model::get_weapons_data(volInt::polyhedron &model)
{
  weapon_slots_existence = 0;
  cur_weapon_slot_data =
    std::vector<weapon_slot_data>(m3d::weapon_slot::max_slots);
  for(std::size_t cur_slot = 0;
      cur_slot < m3d::weapon_slot::max_slots;
      ++cur_slot)
  {
    cur_weapon_slot_data[cur_slot].location_angle_of_slots = 0;
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      cur_weapon_slot_data[cur_slot].R_slots[cur_coord] = 0;
    }
  }

  if(!weapon_attachment_point)
  {
    return;
  }

  // There are m3d::weapon_slot::max_slots weapons in all vangers models.
  // cur_weapon_verts contains pair of vertices for each weapon.
  // This pair of vertices is later used to get weapons' angles.
  std::vector<std::vector<point*>> cur_weapon_verts;
  cur_weapon_verts.reserve(m3d::weapon_slot::max_slots);



  for(std::size_t cur_slot = 0;
      cur_slot < m3d::weapon_slot::max_slots;
      ++cur_slot)
  {
    try
    {
      cur_weapon_verts.push_back(
        get_ref_points_for_part_of_model(
          model,
          weapon_attachment_point,
          c3d::color::string_to_id::attachment_point,
          cur_slot));
    }
    // If get_ref_points_for_part_of_model failed to find reference points.
    catch(exception::model_ref_points_not_found &)
    {
      cur_weapon_verts.push_back(std::vector<point*>(3, nullptr));
    }
  }



  // Getting info about slots.

  for(std::size_t cur_slot = 0;
      cur_slot < m3d::weapon_slot::max_slots;
      ++cur_slot)
  {
    if(cur_weapon_verts[cur_slot][0] &&
       cur_weapon_verts[cur_slot][1] &&
       cur_weapon_verts[cur_slot][2])
    {

      // TEST
      /*
      std::cout << "cur_weapon_verts[" << cur_slot << "][0]: " <<
        (*cur_weapon_verts[cur_slot][0])[0] << "; " <<
        (*cur_weapon_verts[cur_slot][0])[1] << "; " <<
        (*cur_weapon_verts[cur_slot][0])[2] << '\n';
      std::cout << "cur_weapon_verts[" << cur_slot << "][1]: " <<
        (*cur_weapon_verts[cur_slot][1])[0] << "; " <<
        (*cur_weapon_verts[cur_slot][1])[1] << "; " <<
        (*cur_weapon_verts[cur_slot][1])[2] << '\n';
      std::cout << "cur_weapon_verts[" << cur_slot << "][2]: " <<
        (*cur_weapon_verts[cur_slot][2])[0] << "; " <<
        (*cur_weapon_verts[cur_slot][2])[1] << "; " <<
        (*cur_weapon_verts[cur_slot][2])[2] << '\n';
      */

      std::pair<point, point> compare_points =
        get_compare_points(
          cur_weapon_verts[cur_slot],
          weapon_attachment_point);

      weapon_slots_existence |= (1 << cur_slot);
      cur_weapon_slot_data[cur_slot].exists = true;

      point two_rel_to_one =
        volInt::vector_minus((*cur_weapon_verts[cur_slot][1]),
                             (*cur_weapon_verts[cur_slot][0]));

      // If y coordinate of points 2 and 3 relative to 1 is the same
      // in orig model and current model
      // then model wasn't rotated by x or z axes.
      // compare_points.first is difference between 2nd reference points.
      // compare_points.second is difference between 3rd ones.
      // Throwing exception if difference of y coordinate is not zero.
      if(!(std::abs(compare_points.first[1]) <
             volInt::ref_points_distinct_distance &&
           std::abs(compare_points.second[1]) <
             volInt::ref_points_distinct_distance))
      {
        throw std::runtime_error(
          input_file_name_error + " file " +
          model.wavefront_obj_path + "\n" +
          "Weapon " + std::to_string(cur_slot + 1) +
          " was rotated by x or z axis, scaled or changed." + "\n" +
          "Only moving or rotating by y axis is allowed " +
          "for attachment point in mechos model." + '\n');
      }

      cur_weapon_slot_data[cur_slot].location_angle_of_slots =
        std::remainder(
          std::atan2(two_rel_to_one[0], two_rel_to_one[2]) -
            weapon_attachment_point->ref_angle,
          2 * M_PI);



      std::vector<point> ref_verts_rotated
      {
        *weapon_attachment_point->ref_vert_one,
        *weapon_attachment_point->ref_vert_two,
        *weapon_attachment_point->ref_vert_three,
      };
      for(auto &&ref_vert : ref_verts_rotated)
      {
        rotate_3d_point_by_axis(
          ref_vert,
          cur_weapon_slot_data[cur_slot].location_angle_of_slots,
          rotation_axis::y);
      }

      std::vector<point*> ref_verts_rotated_ptr
      {
        &ref_verts_rotated[0],
        &ref_verts_rotated[1],
        &ref_verts_rotated[2],
      };

      std::pair<point, point> rotated_compare_points =
        get_compare_points(
          cur_weapon_verts[cur_slot],
          ref_verts_rotated_ptr);

      // If all coordinates of points 2 and 3 relative to 1 are the same
      // in rotated orig model and current model then model wasn't changed.
      // rotated_compare_points.first is difference between 2nd reference points.
      // rotated_compare_points.second is difference between 3rd ones.
      // Throwing exception if difference is not zero.
      for(std::size_t cur_coord = 0 ; cur_coord < 3; ++cur_coord)
      {
        if(!(std::abs(rotated_compare_points.first[cur_coord]) <
               volInt::ref_points_distinct_distance &&
             std::abs(rotated_compare_points.second[cur_coord]) <
               volInt::ref_points_distinct_distance))
        {
          throw std::runtime_error(
            input_file_name_error + " file " +
            model.wavefront_obj_path + "\n" +
            "Weapon " + std::to_string(cur_slot + 1) +
            " was scaled or changed." + "\n" +
            "Only moving or rotating by y axis is allowed " +
            "for attachment point in mechos model." + '\n');
        }
      }

      cur_weapon_slot_data[cur_slot].R_slots =
        volInt::vector_minus((*cur_weapon_verts[cur_slot][0]),
                             ref_verts_rotated[0]);
    }
  }
}





std::unordered_map<int, volInt::polyhedron>
  wavefront_obj_to_m3d_model::get_wheels_steer(
    volInt::polyhedron &main_model)
{
  std::unordered_map<int, volInt::polyhedron> wheels_steer_models;
  std::unordered_map<int, std::size_t> cur_vert_nums;
  std::unordered_map<int, std::size_t> cur_norm_nums;
  std::unordered_map<int, std::size_t> cur_poly_nums;
  std::unordered_map<int, std::unordered_map<int, int>> vertices_maps;
  std::unordered_map<int, std::unordered_map<int, int>> norms_maps;
  int verts_per_poly = main_model.faces[0].numVerts;
  // TEST
//  std::unordered_map<int, std::ofstream> test_poly_properties_ofstream;

  for(const auto wheel_steer_num : main_model.wheels_steer)
  {
    wheels_steer_models.emplace(wheel_steer_num, volInt::polyhedron());

    if(main_model.wheels_non_ghost.count(wheel_steer_num))
    {
      volInt::polyhedron &cur_wheel_model =
        wheels_steer_models[wheel_steer_num];

      cur_wheel_model.faces.reserve(main_model.numFaces);

      cur_vert_nums[wheel_steer_num] = 0;
      cur_norm_nums[wheel_steer_num] = 0;
      cur_poly_nums[wheel_steer_num] = 0;

      // TEST
//    test_poly_properties_ofstream[wheel_steer_num] =
//      std::ofstream((output_dir_path.string() + "/" +
//                       model_name + "_wheel_before_" +
//                       std::to_string(wheel_steer_num) + ".txt").c_str(),
//                    std::ios_base::out);
    }
  }

  // Copying right polygons to wheel models
  // and making verts and norms maps.
  for(std::size_t cur_poly_num = 0;
      cur_poly_num < main_model.numFaces;
      ++cur_poly_num)
  {
    volInt::face &cur_poly = main_model.faces[cur_poly_num];
    if(cur_poly.color_id == c3d::color::string_to_id::wheel &&
       main_model.wheels_steer.count(cur_poly.wheel_weapon_id) &&
       main_model.wheels_non_ghost.count(cur_poly.wheel_weapon_id))
    {
      volInt::polyhedron &cur_wheel_model =
        wheels_steer_models[cur_poly.wheel_weapon_id];
      int wheel_n = cur_poly.wheel_weapon_id;

      cur_wheel_model.faces.push_back(cur_poly);
      volInt::face &cur_wheel_poly =
        cur_wheel_model.faces[cur_poly_nums[wheel_n]];
      // TEST
//    test_poly_properties_ofstream[wheel_n] << "cur_poly.norm[VOLINT_X]: " <<
//      cur_poly.norm[VOLINT_X] << '\n';
//    test_poly_properties_ofstream[wheel_n] << "cur_poly.norm[VOLINT_Y]: " <<
//      cur_poly.norm[VOLINT_Y] << '\n';
//    test_poly_properties_ofstream[wheel_n] << "cur_poly.norm[VOLINT_Z]: " <<
//      cur_poly.norm[VOLINT_Z] << '\n';
//    test_poly_properties_ofstream[wheel_n] << "cur_poly.w: " <<
//      cur_poly.w << '\n';

      // Checking if vertices and norms indexes are already in the maps
      // and adding them if they are not.
      // Key of the map is original vertex/norm index.
      // Value of the map is new vertex/norm index.
      for(std::size_t cur_vert_num = 0;
          cur_vert_num < main_model.numVertsPerPoly;
          ++cur_vert_num)
      {
        if(vertices_maps[wheel_n].count(cur_wheel_poly.verts[cur_vert_num]))
        {
          // One of the keys of the map is equal to vertex index.
          // Changing index of vertex to value of the map entry.
          cur_wheel_poly.verts[cur_vert_num] =
            vertices_maps[wheel_n][cur_wheel_poly.verts[cur_vert_num]];
        }
        else
        {
          // Vertex index is not found as key in the map.
          // Inserting new key-value pair in the map.
          // Changing index of vertex to size of the map.
          cur_wheel_poly.verts[cur_vert_num] = cur_vert_nums[wheel_n];
          vertices_maps[wheel_n][cur_poly.verts[cur_vert_num]] =
            cur_vert_nums[wheel_n];
          ++cur_vert_nums[wheel_n];
        }

        if(norms_maps[wheel_n].count(cur_wheel_poly.vertNorms[cur_vert_num]))
        {
          // One of the keys of the map is equal to norm index.
          // Changing index of norm to value of the map entry.
          cur_wheel_poly.vertNorms[cur_vert_num] =
            norms_maps[wheel_n][cur_wheel_poly.vertNorms[cur_vert_num]];
        }
        else
        {
          // Norm index is not found as key in the map.
          // Inserting new key-value pair in the map.
          // Changing index of norm to size of the map.
          cur_wheel_poly.vertNorms[cur_vert_num] = cur_norm_nums[wheel_n];
          norms_maps[wheel_n][cur_poly.vertNorms[cur_vert_num]] =
            cur_norm_nums[wheel_n];
          ++cur_norm_nums[wheel_n];
        }
      }
      ++cur_poly_nums[wheel_n];
    }
  }

  // Copying vertices and norms into wheel models.
  // Maps created earlier are used to figure out which vertex to copy where.
  for(const auto wheel_steer_num : main_model.wheels_steer)
  {
    if(main_model.wheels_non_ghost.count(wheel_steer_num))
    {
      volInt::polyhedron &cur_wheel_model =
        wheels_steer_models[wheel_steer_num];

      cur_wheel_model.numVerts = vertices_maps[wheel_steer_num].size();
      cur_wheel_model.numVertNorms = norms_maps[wheel_steer_num].size();
      cur_wheel_model.numFaces = cur_wheel_model.faces.size();
      cur_wheel_model.numVertTotal =
        cur_wheel_model.numFaces * cur_wheel_model.faces[0].numVerts;
      cur_wheel_model.numVertsPerPoly = cur_wheel_model.faces[0].numVerts;

      cur_wheel_model.verts =
        std::vector<std::vector<double>>(cur_wheel_model.numVerts);
      cur_wheel_model.vertNorms =
        std::vector<std::vector<double>>(cur_wheel_model.numVertNorms);

      cur_wheel_model.wavefront_obj_path = main_model.wavefront_obj_path;
      cur_wheel_model.wheel_id = wheel_steer_num;

      // Copying vertices and norms.
      for(const auto &cur_wheel_vertex_ind : vertices_maps[wheel_steer_num])
      {
        cur_wheel_model.verts[cur_wheel_vertex_ind.second] =
          main_model.verts[cur_wheel_vertex_ind.first];
      }
      for(const auto &cur_wheel_norm_ind : norms_maps[wheel_steer_num])
      {
        cur_wheel_model.vertNorms[cur_wheel_norm_ind.second] =
          main_model.vertNorms[cur_wheel_norm_ind.first];
      }

      // Moving coordinate system to wheel center.
      // So center of wheel model will have coordinates: 0, 0, 0.
      point wheel_center = cur_wheel_model.get_model_center();
      cur_wheel_model.offset_point() = wheel_center;
      cur_wheel_model.move_coord_system_to_point(wheel_center);
      // Must be called since model was moved.
      cur_wheel_model.faces_calc_params();
    }
  }


  return wheels_steer_models;
}



void wavefront_obj_to_m3d_model::get_m3d_extreme_points(
  const volInt::polyhedron *main_model,
  const std::unordered_map<int, volInt::polyhedron> *wheels_models)
{
  extreme_points.get_most_extreme(main_model->extreme_points);
  // TEST
  /*
  if(model_name == "m4")
  {
    std::cout << "\n--------------------\n";
    std::cout << "before\n";
    std::cout << "max_point:";
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      std::cout << " " << max_point()[cur_coord];
    }
    std::cout << '\n';
    std::cout << "min_point:";
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      std::cout << " " << min_point()[cur_coord];
    }
    std::cout << '\n';
  }
  */

  if(wheels_models)
  {
    for(const int wheel_steer_num : main_model->wheels_steer)
    {
      if(main_model->wheels_non_ghost.count(wheel_steer_num))
      {
        const volInt::polyhedron &cur_wheel =
          (*wheels_models).at(wheel_steer_num);
        volInt::model_extreme_points cur_wheel_extreme_points =
          cur_wheel.extreme_points;
        volInt::vector_plus_self(cur_wheel_extreme_points.max(),
                                 cur_wheel.offset_point());
        volInt::vector_plus_self(cur_wheel_extreme_points.min(),
                                 cur_wheel.offset_point());
        extreme_points.get_most_extreme(cur_wheel_extreme_points);
      }
    }
  }
  // TEST
  /*
  if(model_name == "m4")
  {
    std::cout << "\n--------------------\n";
    std::cout << "after\n";
    std::cout << "max_point:";
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      std::cout << " " << max_point()[cur_coord];
    }
    std::cout << '\n';
    std::cout << "min_point:";
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      std::cout << " " << min_point()[cur_coord];
    }
    std::cout << '\n';
  }
  */
}

void wavefront_obj_to_m3d_model::get_a3d_extreme_points(
  const std::deque<volInt::polyhedron> *models)
{
  for(const auto &model : *models)
  {
    extreme_points.get_most_extreme(model.extreme_points);
  }
  // TEST
  /*
  if(model_name == "a1")
  {
    std::cout << "\n--------------------\n";
    std::cout << "max_point:";
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      std::cout << " " << max_point()[cur_coord];
    }
    std::cout << '\n';
    std::cout << "min_point:";
    for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
    {
      std::cout << " " << min_point()[cur_coord];
    }
    std::cout << '\n';
  }
  */
}



void wavefront_obj_to_m3d_model::get_m3d_header_data(
  volInt::polyhedron *main_model,
  volInt::polyhedron *main_bound_model,
  std::unordered_map<int, volInt::polyhedron> *wheels_models,
  std::deque<volInt::polyhedron> *debris_models,
  std::deque<volInt::polyhedron> *debris_bound_models)
{
  // TEST
//std::cout << "\n\n\n";
//std::cout << "getting m3d header data for " << model_name << '\n';

  // TEST
  /*
  std::cout << "\n\n\n";
  std::cout << model_name << " main model." << '\n';
  std::cout << "\n\n\n";
  std::cout << model_name << " main bound model." << '\n';
  */

  main_model->calculate_c3d_properties();
  main_bound_model->calculate_c3d_properties();
  if(wheels_models)
  {
    for(int wheel_n : main_model->wheels_steer)
    {
      // TEST
//    std::cout << "\n\n\n";
//    std::cout << "wheel: " << wheel_model.first;
//    std::cout << model_name << " wheel #" << wheel_n << " model." << '\n';
      if(main_model->wheels_non_ghost.count(wheel_n))
      {
        // TEST
//      wheel_model.second.calculate_c3d_properties();
        // TEST
//        std::ofstream test_poly_properties_ofstream_two(
//          (output_dir_path.string() + "/" + model_name +
//             "_wheel_after_" + std::to_string(wheel_n) + ".txt").c_str(),
//          std::ios_base::out);
//        std::cout << "wheel " << wheel_n << " of " <<
//          input_file_path.string() << '\n';
//        for(int cur_face = 0, num_faces = (*wheels_models)[wheel_n].numFaces;
//            cur_face < num_faces;
//            ++cur_face)
//        {
//          volInt::face &cur_poly =
//            (*wheels_models)[wheel_n].faces[cur_face];
//          test_poly_properties_ofstream_two << "cur_poly.norm[VOLINT_X]: " <<
//            cur_poly.norm[VOLINT_X] << '\n';
//          test_poly_properties_ofstream_two << "cur_poly.norm[VOLINT_Y]: " <<
//            cur_poly.norm[VOLINT_Y] << '\n';
//          test_poly_properties_ofstream_two << "cur_poly.norm[VOLINT_Z]: " <<
//            cur_poly.norm[VOLINT_Z] << '\n';
//          test_poly_properties_ofstream_two << "cur_poly.w: " <<
//            cur_poly.w << '\n';
//        }

        (*wheels_models)[wheel_n].calculate_c3d_properties();
      }
    }
  }

  if(debris_models && debris_bound_models)
  {

    int cur_debris = 0;
    int cur_debris_bound = 0;

    for(auto &&model : *debris_models)
    {

      // TEST
//    std::cout << "\n\n\n";
//    std::cout << "debris: " << cur_debris;
//    std::cout << model_name <<
//      " debris #" << cur_debris << " model." << '\n';
      ++cur_debris;
      model.calculate_c3d_properties();
    }
    for(auto &&model : *debris_bound_models)
    {

      // TEST
//    std::cout << "\n\n\n";
//    std::cout << "cur_debris_bound: " << cur_debris_bound;
//    std::cout << model_name <<
//      " debris bound #" << cur_debris << " model." << '\n';
      ++cur_debris_bound;
      model.calculate_c3d_properties();
    }
  }
  // TEST
//std::cout << "\n\n";



//  xmax = main_model->xmax;
//  ymax = main_model->ymax;
//  zmax = main_model->zmax;
  get_m3d_extreme_points(main_model, wheels_models);
  // rmax must be set in get_m3d_scale_size() function.
//  rmax = main_model->rmax;

  body_color_offset = main_model->bodyColorOffset;
  body_color_shift = main_model->bodyColorShift;



  // TEST
  /*
  std::cout << "\n\n";
  std::cout << "model: " << model_name << '\n';
  std::cout << "xmax: " << xmax << '\n';
  std::cout << "ymax: " << ymax << '\n';
  std::cout << "zmax: " << zmax << '\n';
  std::cout << "rmax: " << rmax << '\n';

  std::cout << "body_color_offset: " << body_color_offset << '\n';
  std::cout << "body_color_shift: " << body_color_shift << '\n';
  */



// Obtained in get_wheels_data function.
  /*
  if(n_wheels)
  {
    for(i = 0; i < n_wheels; i++)
    {
      cur_wheel_data[i].steer = ;
      cur_wheel_data[i].r = ;
      cur_wheel_data[i].width = ;
      cur_wheel_data[i].radius = ;
      cur_wheel_data[i].bound_index = ;
    }
  }
  */
  // VANGERS SOURCE
  /*
  for(i = 0; i < n_wheels; i++){
    buf > wheels[i].steer > wheels[i].r > wheels[i].width >
      wheels[i].radius > wheels[i].bound_index;
    }
  */

// Obtained in get_weapons_data function.
  // VANGERS SOURCE
  /*
  buf > slots_existence;
  if(slots_existence){
    for(i = 0; i < MAX_SLOTS; i++){
      buf > R_slots[i] > location_angle_of_slots[i];
      data_in_slots[i] = 0;
      }
    }
  */
}



void wavefront_obj_to_m3d_model::get_a3d_header_data(
  std::deque<volInt::polyhedron> *models)
{
  // TEST
//std::cout << "\n\n\n";
//std::cout << "getting a3d header data for " << model_name << '\n';

  // TEST
//int cur_animated = 0;

  // VANGERS SOURCE
//buf > n_models > xmax > ymax > zmax > rmax;
//buf >  body_color_offset > body_color_shift;

  n_models = models->size();
  if(!n_models)
  {
    boost::filesystem::path first_model_path =
      (input_file_path / (model_name + "_1.obj"));
    throw std::runtime_error(
      input_file_name_error + " file " +
      first_model_path.string() + " is not valid *.obj model." + '\n' +
      "Expecting first model of *.a3d to be valid." + '\n');
  }

  for(auto &&model : *models)
  {
    // TEST
//  std::cout << "\n\n\n";
//  std::cout << "cur_animated: " << cur_animated;
//  std::cout << model_name <<
//    " animated #" << cur_animated << " model." << '\n';
//  ++cur_animated;

    model.calculate_c3d_properties();
  }

//  xmax = (*models)[0].xmax;
//  ymax = (*models)[0].ymax;
//  zmax = (*models)[0].zmax;
  get_a3d_extreme_points(models);
  // rmax must be set in get_a3d_scale_size() function.
//  rmax = (*models)[0].rmax;

  body_color_offset = (*models)[0].bodyColorOffset;
  body_color_shift = (*models)[0].bodyColorShift;

  // TEST
  /*
  std::cout << "\n\n";
  std::cout << "model: " << model_name << '\n';
  std::cout << "xmax: " << xmax << '\n';
  std::cout << "ymax: " << ymax << '\n';
  std::cout << "zmax: " << zmax << '\n';
  std::cout << "rmax: " << rmax << '\n';

  std::cout << "body_color_offset: " << body_color_offset << '\n';
  std::cout << "body_color_shift: " << body_color_shift << '\n';
  */

// VANGERS SOURCE
//buf > n_models > xmax > ymax > zmax > rmax;
//buf >  body_color_offset > body_color_shift;
}



void wavefront_obj_to_m3d_model::get_wheels_data(
  const volInt::polyhedron &main_model)
{
  n_wheels = main_model.wheels.size();

  cur_wheel_data = std::vector<wheel_data>(n_wheels);

  // Getting extreme points of wheels to get wheel data.
  std::unordered_map<int, volInt::model_extreme_points> wheels_extreme_points;
  for(std::size_t wheel_n = 0; wheel_n < n_wheels; ++wheel_n)
  {
    wheels_extreme_points[wheel_n] = volInt::model_extreme_points();
  }

  for(const auto &cur_poly : main_model.faces)
  {
    if(cur_poly.color_id == c3d::color::string_to_id::wheel &&
       main_model.wheels.count(cur_poly.wheel_weapon_id))
    {
      for(const auto cur_vert_ind : cur_poly.verts)
      {
        wheels_extreme_points[cur_poly.wheel_weapon_id].
          get_most_extreme(main_model.verts[cur_vert_ind]);
      }
    }
  }

  // Getting wheel data using extreme coords.
  for(std::size_t wheel_n = 0; wheel_n < n_wheels; ++wheel_n)
  {

    if(main_model.wheels_steer.count(wheel_n))
    {
      cur_wheel_data[wheel_n].steer = 1;
    }
    else
    {
      cur_wheel_data[wheel_n].steer = 0;
    }

    cur_wheel_data[wheel_n].r = wheels_extreme_points[wheel_n].get_center();
    cur_wheel_data[wheel_n].width =
      wheels_extreme_points[wheel_n].xmax() -
      wheels_extreme_points[wheel_n].xmin();
    cur_wheel_data[wheel_n].radius =
      wheels_extreme_points[wheel_n].zmax() -
      wheels_extreme_points[wheel_n].zmin() / 2;

    // TEST
    /*
    if(model_name == "m4")
    {
      std::cout << "cur_wheel_data[" << wheel_n << "].width: " <<
        cur_wheel_data[wheel_n].width << '\n';
      std::cout << "cur_wheel_data[" << wheel_n << "].radius: " <<
        cur_wheel_data[wheel_n].radius << '\n';
    }
    */
  }
}



void wavefront_obj_to_m3d_model::get_debris_data(
  const std::deque<volInt::polyhedron> &debris_models,
  const std::deque<volInt::polyhedron> &debris_bound_models)
{
  std::size_t debris_num = debris_models.size();
  std::size_t debris_bound_models_size = debris_bound_models.size();
  if(debris_num > debris_bound_models_size)
  {
    debris_num = debris_bound_models_size;
  }
  n_debris = debris_num;

  if(n_debris)
  {
    std::cout << '\n';
    std::cout << "Found " << std::to_string(debris_num) <<
      " debris for " << input_file_path << " model." << '\n';
  }
  else
  {
    std::cout << '\n';
    std::cout << "Couldn't find debris for " <<
      input_file_path << " model." << '\n';
  }
}





void wavefront_obj_to_m3d_model::get_scale_helper_get_extreme_radius(
  volInt::polyhedron *model,
  double &extreme_radius,
  const point offset)
{
  model->calculate_rmax();
  get_extreme_radius(extreme_radius, model->rmax, offset);
}



void wavefront_obj_to_m3d_model::get_m3d_scale_size(
  volInt::polyhedron *main_model,
  volInt::polyhedron *main_bound_model,
  std::unordered_map<int, volInt::polyhedron> *wheels_models,
  std::deque<volInt::polyhedron> *debris_models,
  std::deque<volInt::polyhedron> *debris_bound_models)
{
  double extreme_radius = 0.0;

  get_scale_helper_get_extreme_radius(main_model, extreme_radius);
//get_scale_helper_get_extreme_radius(main_bound_model,
//                                    extreme_radius);
  if(wheels_models)
  {
    for(const int wheel_steer_num : main_model->wheels_steer)
    {
      if(main_model->wheels_non_ghost.count(wheel_steer_num))
      {
        get_scale_helper_get_extreme_radius(
          &wheels_models->at(wheel_steer_num),
          extreme_radius,
          cur_wheel_data[wheel_steer_num].r);
      }
    }
//  for(const auto &model : *wheels_models)
//  {
//    get_scale_helper_get_extreme_radius(&model.second,
//                                        extreme_radius);
//  }
  }

  // TEST
  /*
  if(boost::filesystem::path(main_model->wavefront_obj_path).stem().string() ==
     "m5_main")
  {
    std::cout << "m5 scaling test" << '\n';
    std::cout << "weapon_slots_existence: " << weapon_slots_existence << '\n';
  }
  */

  if(weapon_slots_existence)
  {
    for(const auto &slot_data : cur_weapon_slot_data)
    {
      if(slot_data.exists)
      {
        get_extreme_radius(extreme_radius,
                           max_weapons_radius,
                           slot_data.R_slots);
/*
        double max_slot_distance =
          std::sqrt(
            slot_data.R_slots[0]*slot_data.R_slots[0] +
            slot_data.R_slots[1]*slot_data.R_slots[1] +
            slot_data.R_slots[2]*slot_data.R_slots[2]);
        get_extreme_radius(extreme_radius, max_slot_distance);
*/
/*
        for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
        {
          get_extreme_radius(
            extreme_radius,
            slot_data.R_slots[cur_coord] + max_weapons_radius);
          get_extreme_radius(
            extreme_radius,
            slot_data.R_slots[cur_coord] - max_weapons_radius);
          // TEST
          if(boost::filesystem::path(main_model->wavefront_obj_path)
               .stem().string() ==
             "m5_main")
          {
            std::cout << "extreme_radius: " <<
              extreme_radius << '\n';
            std::cout << "slot_data.R_slots[" << cur_coord << "]: " <<
              slot_data.R_slots[cur_coord] << '\n';
            std::cout << "max_weapons_radius: " <<
              max_weapons_radius << '\n';
            std::cout <<
              "slot_data.R_slots[cur_coord] + max_weapons_radius: " <<
              slot_data.R_slots[cur_coord] + max_weapons_radius << '\n';
            std::cout <<
              "slot_data.R_slots[cur_coord] - max_weapons_radius: " <<
              slot_data.R_slots[cur_coord] - max_weapons_radius << '\n';
          }
        }
*/
      }
    }
  }
  if(debris_models && debris_bound_models)
  {
    for(auto &model : *debris_models)
    {
      get_scale_helper_get_extreme_radius(&model, extreme_radius);
    }
//  for(const auto &model : *debris_bound_models)
//  {
//    get_scale_helper_get_extreme_radius(&model, extreme_radius);
//  }
  }

  rmax = extreme_radius;
  scale_size = c3d::scaling_max_extreme_radius / extreme_radius;

  if(non_mechos_scale_sizes)
  {
    (*non_mechos_scale_sizes)[model_name] =
      extreme_radius / c3d::scaling_max_extreme_radius;
  }
  else
  {
    prm_scale_size = extreme_radius / c3d::scaling_max_extreme_radius;
  }
}



void wavefront_obj_to_m3d_model::get_a3d_scale_size(
  std::deque<volInt::polyhedron> *models)
{
  double extreme_radius = 0.0;

  for(auto &model : *models)
  {
    get_scale_helper_get_extreme_radius(&model, extreme_radius);
  }

  rmax = extreme_radius;
  scale_size = c3d::scaling_max_extreme_radius / extreme_radius;

  (*non_mechos_scale_sizes)[model_name] =
    extreme_radius / c3d::scaling_max_extreme_radius;
}





std::size_t wavefront_obj_to_m3d_model::get_c3d_file_size(
  const volInt::polyhedron *model)
{
  return
    c3d::header::size +
    model->numVerts * c3d::vertex::size +
    model->numVertNorms * c3d::normal::size +
    model->numFaces *
      (c3d::polygon::general_info_size +
         (model->numVertsPerPoly * c3d::polygon::size_per_vertex) +
         c3d::polygon_sort_info::size);
}



std::size_t wavefront_obj_to_m3d_model::get_m3d_file_size(
  const volInt::polyhedron *main_model,
  const volInt::polyhedron *main_bound_model,
  const std::unordered_map<int, volInt::polyhedron> *wheels_models,
  const std::deque<volInt::polyhedron> *debris_models,
  const std::deque<volInt::polyhedron> *debris_bound_models)
{
  std::size_t size = m3d::header::size +
                     n_wheels * m3d::wheel::size +
                     m3d::weapon_slot::slots_existence_size;
  if(weapon_slots_existence)
  {
    size += m3d::weapon_slot::max_slots * m3d::weapon_slot::slot_data_size;
  }

  size += get_c3d_file_size(main_model);
  size += get_c3d_file_size(main_bound_model);
  if(n_wheels && wheels_models)
  {
    for(std::size_t i = 0; i < n_wheels; ++i)
    {
      if(cur_wheel_data[i].steer)
      {
        size += get_c3d_file_size(&wheels_models->at(i));
      }
    }
  }
  if(n_debris && debris_models && debris_bound_models)
  {
    for(std::size_t i = 0; i < n_debris; ++i)
    {
      size += get_c3d_file_size(&(*debris_models)[i]);
      size += get_c3d_file_size(&(*debris_bound_models)[i]);
    }
  }
  return size;
}



std::size_t wavefront_obj_to_m3d_model::get_a3d_file_size(
  const std::deque<volInt::polyhedron> *models)
{
  std::size_t size = a3d::header::size;
  for(std::size_t cur_model = 0; cur_model < n_models; ++cur_model)
  {
    size += get_c3d_file_size(&(*models)[cur_model]);
  }
  return size;
}





void wavefront_obj_to_m3d_model::remove_polygons_helper_erase_mechos(
  volInt::polyhedron &model)
{
  model.faces.erase(
    std::remove_if(
      model.faces.begin(), model.faces.end(),
      [&](const volInt::face &poly)
      {
        if(poly.color_id == c3d::color::string_to_id::attachment_point ||
           poly.color_id == c3d::color::string_to_id::center_of_mass ||
           (poly.color_id == c3d::color::string_to_id::weapon &&
              poly.wheel_weapon_id >= 0) ||
           (poly.color_id == c3d::color::string_to_id::wheel &&
              (model.wheels_steer.count(poly.wheel_weapon_id) ||
                 model.wheels_ghost.count(poly.wheel_weapon_id))))
        {
          return true;
        }
        return false;
      }
    ),
    model.faces.end()
  );
}



void wavefront_obj_to_m3d_model::remove_polygons_helper_erase_weapons(
  volInt::polyhedron &model)
{
  model.faces.erase(
    std::remove_if(
      model.faces.begin(), model.faces.end(),
      [&](const volInt::face &poly)
      {
        if(poly.color_id == c3d::color::string_to_id::attachment_point ||
           poly.color_id == c3d::color::string_to_id::center_of_mass)
        {
          return true;
        }
        return false;
      }
    ),
    model.faces.end()
  );
}



void wavefront_obj_to_m3d_model::remove_polygons_helper_erase_regular(
  volInt::polyhedron &model)
{
  model.faces.erase(
    std::remove_if(
      model.faces.begin(), model.faces.end(),
      [&](const volInt::face &poly)
      {
        if(poly.color_id == c3d::color::string_to_id::center_of_mass)
        {
          return true;
        }
        return false;
      }
    ),
    model.faces.end()
  );
}



std::vector<int>
  wavefront_obj_to_m3d_model::remove_polygons_helper_create_ind_change_map(
    int size,
    std::unordered_set<int> &verts_to_keep)
{
  std::vector<int> ret(size, 0);
  int number_skip = 0;
  for(int cur_vert = 0; cur_vert < size; ++cur_vert)
  {
    if(!verts_to_keep.count(cur_vert))
    {
      ++number_skip;
    }
    ret[cur_vert] = number_skip;
  }
  return ret;
}



void wavefront_obj_to_m3d_model::remove_polygons(
  volInt::polyhedron &model,
  remove_polygons_model model_type)
{
  std::unordered_set<int> verts_to_keep;
  std::unordered_set<int> norms_to_keep;
  verts_to_keep.reserve(model.verts.size());
  norms_to_keep.reserve(model.vertNorms.size());

  // removing right polygons
  if(model_type == remove_polygons_model::mechos)
  {
    remove_polygons_helper_erase_mechos(model);
  }
  else if(model_type == remove_polygons_model::weapon)
  {
    remove_polygons_helper_erase_weapons(model);
  }
  else if(model_type == remove_polygons_model::regular)
  {
    remove_polygons_helper_erase_regular(model);
  }

  model.numFaces = model.faces.size();


  // Getting list of vertices and normals to erase.
  for(int cur_poly_n = 0; cur_poly_n < model.numFaces; ++cur_poly_n)
  {
    for(int cur_vert = 0; cur_vert < model.numVertsPerPoly; ++cur_vert)
    {
      verts_to_keep.insert(model.faces[cur_poly_n].verts[cur_vert]);
      norms_to_keep.insert(model.faces[cur_poly_n].vertNorms[cur_vert]);
    }
  }

  int cur_vert_n = 0;
  model.verts.erase(
    std::remove_if(
      model.verts.begin(), model.verts.end(),
      [&](const std::vector<double> &)
      {
        return !verts_to_keep.count(cur_vert_n++);
      }
    ),
    model.verts.end()
  );
  int cur_norm_n = 0;
  model.vertNorms.erase(
    std::remove_if(
      model.vertNorms.begin(), model.vertNorms.end(),
      [&](const std::vector<double> &)
      {
        return !norms_to_keep.count(cur_norm_n++);
      }
    ),
    model.vertNorms.end()
  );


  // Updating all existing polygons' indexes
  // since vertices and normals were deleted.
  std::vector<int> new_verts_ind_change_map =
    remove_polygons_helper_create_ind_change_map(model.numVerts,
                                                 verts_to_keep);
  std::vector<int> new_norms_ind_change_map =
    remove_polygons_helper_create_ind_change_map(model.numVertNorms,
                                                 norms_to_keep);
  for(auto &&cur_poly : model.faces)
  {
    for(auto &&vert_ind : cur_poly.verts)
    {
      vert_ind -= new_verts_ind_change_map[vert_ind];
    }
    for(auto &&norm_ind : cur_poly.vertNorms)
    {
      norm_ind -= new_norms_ind_change_map[norm_ind];
    }
  }

  // TEST
  /*
  for(int cur_poly_n = 0; cur_poly_n < model.numFaces; ++cur_poly_n)
  {
    for(int cur_vert = 0; cur_vert < model.numVertsPerPoly; ++cur_vert)
    {
      if(model.faces[cur_poly_n].verts[cur_vert] < 0 ||
         model.faces[cur_poly_n].verts[cur_vert] >= model.verts.size())
      {
        std::cout << "model.faces[" << cur_poly_n << "].verts[" <<
            cur_vert << "]: " <<
          model.faces[cur_poly_n].verts[cur_vert] << '\n';
      }
      if(model.faces[cur_poly_n].vertNorms[cur_vert] < 0 ||
         model.faces[cur_poly_n].vertNorms[cur_vert] >= model.vertNorms.size())
      {
        std::cout << "model.faces[" << cur_poly_n << "].vertNorms[" <<
            cur_vert << "]: " <<
          model.faces[cur_poly_n].vertNorms[cur_vert] << '\n';
      }
    }
  }
  */


  model.numVerts = model.verts.size();
  model.numVertNorms = model.vertNorms.size();
  model.numVertTotal = model.numFaces * model.faces[0].numVerts;
}



void create_game_lst(
  const boost::filesystem::path &input_file_path_arg,
  const boost::filesystem::path &where_to_save_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const std::unordered_map<std::string, double> *non_mechos_scale_sizes_arg)
{

  // TEST
  /*
  std::cout << "game.lst input path: " << input_file_path_arg.string() << '\n';
  std::cout << "game.lst output path: " << where_to_save_arg.string() << '\n';
  */

  std::string orig_game_lst_data = read_file(
    input_file_path_arg,
    TRACTOR_CONVERTER_BINARY,
    0,
    0,
    TRACTOR_CONVERTER_FILE_READ_ALL,
    input_file_name_error_arg);

  sicher_cfg_writer cur_cfg_writer(
    std::move(orig_game_lst_data),
    0,
    input_file_path_arg.string(),
    input_file_name_error_arg,
    non_mechos_scale_sizes_arg->size() * sicher_game_lst_int_size_increase);

  const int max_model = cur_cfg_writer.get_next_value<int>("NumModel");
  const int max_size = cur_cfg_writer.get_next_value<int>("MaxSize");
  // TEST
//std::cout << "max_model: " << max_model << '\n';
//std::cout << "max_size: " << max_size << '\n';
  for(int cur_model = 0; cur_model < max_model; ++cur_model)
  {
    const int model_num = cur_cfg_writer.get_next_value<int>("ModelNum");
    // TEST
//  std::cout << "cur_model: " << cur_model << '\n';
//  std::cout << "model_num: " << model_num << '\n';
    if(cur_model != model_num)
    {
      throw std::runtime_error(
        input_file_name_error_arg + " file " +
        input_file_path_arg.string() + " unexpected ModelNum " +
        std::to_string(model_num) + ".\n" +
        "Expected " + std::to_string(cur_model) + ".\n" +
        "Note: ModelNum values must be placed " +
        "in order from 0 to (NumModel - 1).\n" +
        "ModelNum is current model, NumModel is total number of models.\n");
    }
    const std::string model_path =
      cur_cfg_writer.get_next_value<std::string>("Name");
    const std::string model_name =
      boost::filesystem::path(model_path).stem().string();

    // TEST
//  std::cout << "model_name: " << model_name << '\n';
//  std::cout << "boost::filesystem::path(model_name).stem().string(): " <<
//    boost::filesystem::path(model_name).stem().string() << '\n';

    if((*non_mechos_scale_sizes_arg).count(model_name))
    {
      const double scale_size = (*non_mechos_scale_sizes_arg).at(model_name);
      const int scale_size_game_lst =
        round_half_to_even<double, int>(scale_size * max_size);
      cur_cfg_writer.overwrite_next_value("Size", scale_size_game_lst, "%i");
    }
  }

  cur_cfg_writer.write_until_end();

  save_file(where_to_save_arg,
            cur_cfg_writer.out_str(),
            TRACTOR_CONVERTER_BINARY,
            output_file_name_error_arg);
}



void create_prm(
  const boost::filesystem::path &input_file_path_arg,
  const boost::filesystem::path &where_to_save_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const double scale_size)
{
  // TEST
  /*
  std::cout << "prm input path: " << input_file_path_arg.string() << '\n';
  std::cout << "prm output path: " << where_to_save_arg.string() << '\n';
  */

  std::string orig_prm_data = read_file(
    input_file_path_arg,
    TRACTOR_CONVERTER_BINARY,
    0,
    0,
    TRACTOR_CONVERTER_FILE_READ_ALL,
    input_file_name_error_arg);

  sicher_cfg_writer cur_cfg_writer(
    std::move(orig_prm_data),
    0,
    input_file_path_arg.string(),
    input_file_name_error_arg,
    sicher_prm_float_size_increase);

  cur_cfg_writer.overwrite_next_value(
    "scale_size:",
    scale_size,
    sprintf_float_sicher_cfg_format);
  cur_cfg_writer.write_until_end();

  save_file(where_to_save_arg,
            cur_cfg_writer.out_str(),
            TRACTOR_CONVERTER_BINARY,
            output_file_name_error_arg);
}



void mechos_wavefront_objs_to_m3d(
  const boost::filesystem::path &input_file_path_arg,
  const boost::filesystem::path &output_dir_path_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const volInt::polyhedron *example_weapon_model_arg,
  const volInt::polyhedron *weapon_attachment_point_model_arg,
  const volInt::polyhedron *center_of_mass_model_arg,
  double max_weapons_radius_arg,
  unsigned int c3d_default_material_id_arg)
{
  wavefront_obj_to_m3d_model cur_vangers_model(
    input_file_path_arg,
    output_dir_path_arg,
    input_file_name_error_arg,
    output_file_name_error_arg,
    example_weapon_model_arg,
    weapon_attachment_point_model_arg,
    center_of_mass_model_arg,
    max_weapons_radius_arg,
    c3d_default_material_id_arg,
    nullptr);
  cur_vangers_model.mechos_wavefront_objs_to_m3d();
}



volInt::polyhedron weapon_wavefront_objs_to_m3d(
  const boost::filesystem::path &input_file_path_arg,
  const boost::filesystem::path &output_dir_path_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const volInt::polyhedron *weapon_attachment_point_arg,
  const volInt::polyhedron *center_of_mass_model_arg,
  unsigned int c3d_default_material_id_arg,
  std::unordered_map<std::string, double> *non_mechos_scale_sizes_arg)
{
  wavefront_obj_to_m3d_model cur_vangers_model(
    input_file_path_arg,
    output_dir_path_arg,
    input_file_name_error_arg,
    output_file_name_error_arg,
    nullptr,
    weapon_attachment_point_arg,
    center_of_mass_model_arg,
    0.0,
    c3d_default_material_id_arg,
    non_mechos_scale_sizes_arg);
  return cur_vangers_model.weapon_wavefront_objs_to_m3d();
}



void animated_wavefront_objs_to_a3d(
  const boost::filesystem::path &input_file_path_arg,
  const boost::filesystem::path &output_dir_path_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const volInt::polyhedron *center_of_mass_model_arg,
  unsigned int c3d_default_material_id_arg,
  std::unordered_map<std::string, double> *non_mechos_scale_sizes_arg)
{
  wavefront_obj_to_m3d_model cur_vangers_model(
    input_file_path_arg,
    output_dir_path_arg,
    input_file_name_error_arg,
    output_file_name_error_arg,
    nullptr,
    nullptr,
    center_of_mass_model_arg,
    0.0,
    c3d_default_material_id_arg,
    non_mechos_scale_sizes_arg);
  cur_vangers_model.animated_wavefront_objs_to_a3d();
}



void other_wavefront_objs_to_m3d(
  const boost::filesystem::path &input_file_path_arg,
  const boost::filesystem::path &output_dir_path_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const volInt::polyhedron *center_of_mass_model_arg,
  unsigned int c3d_default_material_id_arg,
  std::unordered_map<std::string, double> *non_mechos_scale_sizes_arg)
{
  wavefront_obj_to_m3d_model cur_vangers_model(
    input_file_path_arg,
    output_dir_path_arg,
    input_file_name_error_arg,
    output_file_name_error_arg,
    nullptr,
    nullptr,
    center_of_mass_model_arg,
    0.0,
    c3d_default_material_id_arg,
    non_mechos_scale_sizes_arg);
  cur_vangers_model.other_wavefront_objs_to_m3d();
}



} // namespace helpers
} // namespace tractor_converter
