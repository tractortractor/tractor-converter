#include "vangers_3d_model_operations.hpp"



namespace tractor_converter{
namespace helpers{



wheel_data::wheel_data()
{
  steer = 0;
  r = std::vector<double>(3, 0.0);
  width = 0;
  radius = 0;
}

weapon_slot_data::weapon_slot_data()
{
  R_slot = std::vector<double>(3, 0.0);
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
