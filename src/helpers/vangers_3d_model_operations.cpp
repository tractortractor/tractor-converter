#include "vangers_3d_model_operations.hpp"



namespace tractor_converter{
namespace helpers{



wheel_data::wheel_data()
{
  steer = 0;
  r = std::vector<double>(volInt::axes_num, 0.0);
  width = 0;
  radius = 0;
}

weapon_slot_data::weapon_slot_data()
{
  R_slot = std::vector<double>(volInt::axes_num, 0.0);
  location_angle_of_slot = 0.0;
  exists = false;
}



vangers_model::vangers_model(
  const boost::filesystem::path &input_file_path_arg,
  const boost::filesystem::path &output_dir_path_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const volInt::polyhedron *example_weapon_model_arg,
  const volInt::polyhedron *weapon_attachment_point_arg,
  const volInt::polyhedron *ghost_wheel_model_arg,
  const volInt::polyhedron *center_of_mass_model_arg)
: input_file_path(input_file_path_arg),
  output_dir_path(output_dir_path_arg),
  input_file_name_error(input_file_name_error_arg),
  output_file_name_error(output_file_name_error_arg),
  example_weapon_model(example_weapon_model_arg),
  weapon_attachment_point(weapon_attachment_point_arg),
  ghost_wheel_model(ghost_wheel_model_arg),
  center_of_mass_model(center_of_mass_model_arg)
{
  scale_size = 0.0;

  n_wheels = 0;
  n_debris = 0;
  n_models = 0;

  rmax = 0.0;

  body_color_offset = 0;
  body_color_shift = 0;

  weapon_slots_existence = 0;
}





void vangers_model::merge_helper_move_model_into_main(
  volInt::polyhedron &main_model,
  volInt::polyhedron &model_to_move,
  point new_position,
  double new_angle,
  int wheel_weapon_num,
  merge_model_type merge_type) const
{
  if(model_to_move.verts.size() == 0)
  {
    return;
  }

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
  model_to_move.set_color_id(merge_model_color_id, wheel_weapon_num);



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



// Not used.
void vangers_model::merge_main_model_with_weapons(
  volInt::polyhedron &main_model,
  bitflag<merge_with_weapons_flag> flags) const
{
  // inserting weapon models into main model
  for(int cur_weapon_num = 0;
      cur_weapon_num < m3d::weapon_slot::max_slots;
      ++cur_weapon_num)
  {
    if(flags & merge_with_weapons_flag::extract_nonexistent_weapons ||
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
void vangers_model::merge_model_with_center_of_mass(
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



void vangers_model::merge_model_with_weapon_attachment_point(
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



volInt::model_extreme_points vangers_model::get_wheel_params_extremes()
{
  volInt::model_extreme_points params_extremes;

  for(auto wheel_data_el : cur_wheel_data)
  {
    std::vector<double> wheel_max_point = wheel_data_el.r;
    std::vector<double> wheel_min_point = wheel_data_el.r;
    for(auto cur_axis : volInt::axes_by_plane[VOLINT_X])
    {
      wheel_max_point[cur_axis] += wheel_data_el.radius;
      wheel_min_point[cur_axis] -= wheel_data_el.radius;
    }
    wheel_max_point[VOLINT_X] += wheel_data_el.width / 2;
    wheel_min_point[VOLINT_X] -= wheel_data_el.width / 2;

    params_extremes.get_most_extreme_cmp_cur(wheel_max_point);
    params_extremes.get_most_extreme_cmp_cur(wheel_min_point);
  }

  return params_extremes;
}





// not needed
/*
void vangers_model::scale_3d_point(std::vector<double> &point)
{
  volInt::vector_multiply_self(point, scale_size);
}



void vangers_model::scale_c3d(volInt::polyhedron &c3d_model)
{
  for(auto &&vert_to_change : c3d_model.verts)
  {
    scale_3d_point(vert_to_change);
  }

  volInt::vector_multiply_self(c3d_model.max_point(), scale_size);

  volInt::vector_multiply_self(c3d_model.min(), scale_size);

  volInt::vector_multiply_self(c3d_model.offset_point(), scale_size);

  c3d_model.rmax *= scale_size;

  c3d_model.volume *= std::pow(scale_size, 3);

  scale_3d_point(c3d_model.rcm);
}
*/


std::pair<std::vector<double>, std::vector<double>> &
  vangers_model::extreme_points_pair()
{
  return extreme_points.extreme_points_pair;
}

const std::pair<std::vector<double>, std::vector<double>> &
  vangers_model::extreme_points_pair() const
{
  return extreme_points.extreme_points_pair;
}

std::vector<double> &vangers_model::max_point()
{
  return extreme_points.max();
}

const std::vector<double> &vangers_model::max_point() const
{
  return extreme_points.max();
}

std::vector<double> &vangers_model::min_point()
{
  return extreme_points.min();
}

const std::vector<double> &vangers_model::min_point() const
{
  return extreme_points.min();
}


double vangers_model::xmax() const
{
  return extreme_points.xmax();
}

double vangers_model::ymax() const
{
  return extreme_points.ymax();
}

double vangers_model::zmax() const
{
  return extreme_points.zmax();
}


double vangers_model::xmin() const
{
  return extreme_points.xmin();
}

double vangers_model::ymin() const
{
  return extreme_points.ymin();
}

double vangers_model::zmin() const
{
  return extreme_points.zmin();
}


void vangers_model::set_xmax(double new_xmax)
{
  extreme_points.set_xmax(new_xmax);
}

void vangers_model::set_ymax(double new_ymax)
{
  extreme_points.set_ymax(new_ymax);
}

void vangers_model::set_zmax(double new_zmax)
{
  extreme_points.set_zmax(new_zmax);
}


void vangers_model::set_xmin(double new_xmin)
{
  extreme_points.set_xmin(new_xmin);
}

void vangers_model::set_ymin(double new_ymin)
{
  extreme_points.set_ymin(new_ymin);
}

void vangers_model::set_zmin(double new_zmin)
{
  extreme_points.set_zmin(new_zmin);
}



} // namespace helpers
} // namespace tractor_converter
