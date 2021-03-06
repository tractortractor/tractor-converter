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
  const boost::filesystem::path &input_m3d_path_arg,
  const boost::filesystem::path &output_m3d_path_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const volInt::polyhedron *example_weapon_model_arg,
  const volInt::polyhedron *weapon_attachment_point_arg,
  const volInt::polyhedron *ghost_wheel_model_arg,
  const volInt::polyhedron *center_of_mass_model_arg)
: input_m3d_path(input_m3d_path_arg),
  output_m3d_path(output_m3d_path_arg),
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





std::string vangers_model::file_prefix_to_filename(
  const std::string &prefix,
  const std::size_t *model_num)
{
  std::string full_prefix;
  if(prefix.empty())
  {
    full_prefix = model_name;
  }
  else
  {
    full_prefix = model_name + "_" + prefix;
  }
  std::string model_num_string;
  if(model_num)
  {
    model_num_string = "_" + std::to_string(*model_num + 1);
  }
  return full_prefix + model_num_string + ext::obj;
}





void vangers_model::merge_helper_move_model_into_main(
  volInt::polyhedron &main_model,
  volInt::polyhedron &model_to_move,
  point new_position,
  double new_angle,
  int wheel_id,
  int weapon_id,
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
    merge_model_color_id = c3d::color::string_to_id::invalid_color_id;
  }
  else if(merge_type == merge_model_type::attachment_point)
  {
    merge_model_color_id = c3d::color::string_to_id::attachment_point;
  }
  // Changing color_id for model.
  model_to_move.set_color_id(merge_model_color_id, wheel_id, weapon_id);



  // Changing model_to_move's y angle.
  model_to_move.rotate_by_axis(new_angle, volInt::rotation_axis::y);



  // Changing coordinates of all vertices of model_to_move
  // so it will be in the right place.
  // new_position is center coordinates of model_to_move
  // relative to main model center.
  model_to_move.move_model_to_point(new_position);

  // Since all vertices and normals are appended to main model,
  // all vertices' and normals' indices of moved polygons must be updated.
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

  // Moving vertices, vertices' normals and polygons of wheel into main model.
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
    volInt::invalid::wheel_id,
    volInt::invalid::weapon_id,
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
