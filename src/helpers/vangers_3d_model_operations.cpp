#include "vangers_3d_model_operations.hpp"



namespace tractor_converter{
namespace helpers{



double sicher_angle_to_radians(int sicher_angle)
{
  return static_cast<double>(sicher_angle) *
         (M_PI/static_cast<double>(sicher_angle_Pi));
}

int radians_to_sicher_angle(double radians)
{
  return std::round(radians*(static_cast<double>(sicher_angle_Pi)/M_PI));
}



/*
// VANGERS SOURCE
// how rotation works in vangers source
inline Vector Vector::operator* (const DBM& m) const
{
  return
    Vector(
      round(m.a[0]*x + m.a[1]*y + m.a[2]*z),
      round(m.a[3]*x + m.a[4]*y + m.a[5]*z),
      round(m.a[6]*x + m.a[7]*y + m.a[8]*z));
}

inline DBM::DBM(int angle,int axis)
{
//------ Calculate Matrix for ROTATE point an angle ------
  double calpha = Cos(angle);
  double salpha = Sin(angle);
  switch(axis){
    case Z_AXIS:
      a[0]   =  calpha; a[1]   = -salpha; a[2]   = 0;
      a[3]   =  salpha; a[4]   = calpha;  a[5]   = 0;
      a[6]   = 0;    a[7]   = 0;      a[8]   = 1;
      break;
    case X_AXIS:
      a[0]   =  1;    a[1]   =  0;      a[2]   = 0;
      a[3]   =  0;    a[4]   =  calpha; a[5]   = -salpha;
      a[6]   =  0;    a[7]   =  salpha; a[8]   = calpha;
      break;
    case Y_AXIS:
      a[0]   = calpha;  a[1]   =  0;      a[2]   = salpha;
      a[3]   = 0;    a[4]   =  1;      a[5]   = 0;
      a[6]   = -salpha; a[7]   =  0;      a[8]   = calpha;
      break;
    }
}
*/

void rotate_3d_point_by_axis(
  std::vector<double> &point,
  double angle_sin,
  double angle_cos,
  rotation_axis axis)
{
  std::vector<double> point_orig = point;
  if(axis == rotation_axis::x)
  {
    point[1] = angle_cos * point_orig[1] - angle_sin * point_orig[2];
    point[2] = angle_sin * point_orig[1] + angle_cos * point_orig[2];
  }
  if(axis == rotation_axis::y)
  {
    point[0] =  angle_cos * point_orig[0] + angle_sin * point_orig[2];
    point[2] = -angle_sin * point_orig[0] + angle_cos * point_orig[2];
  }
  if(axis == rotation_axis::z)
  {
    point[0] = angle_cos * point_orig[0] - angle_sin * point_orig[1];
    point[1] = angle_sin * point_orig[0] + angle_cos * point_orig[1];
  }
}

void rotate_3d_point_by_axis(std::vector<double> &point,
                             double angle,
                             rotation_axis axis)
{
  if(angle == 0.0)
  {
    return;
  }
  double angle_sin = std::sin(angle);
  double angle_cos = std::cos(angle);
  rotate_3d_point_by_axis(point, angle_sin, angle_cos, axis);
}

void rotate_c3d_by_axis(volInt::polyhedron &c3d_model,
                        double angle,
                        rotation_axis axis)
{
  if(angle == 0.0)
  {
    return;
  }
  double angle_sin = std::sin(angle);
  double angle_cos = std::cos(angle);
  for(auto &&vert_to_change : c3d_model.verts)
  {
    rotate_3d_point_by_axis(vert_to_change, angle_sin, angle_cos, axis);
  }
  for(auto &&vert_norm_to_change : c3d_model.vertNorms)
  {
    rotate_3d_point_by_axis(vert_norm_to_change, angle_sin, angle_cos, axis);
  }
}



std::vector<double> normalize(double norm, const std::vector<double> &vec)
{
  double vec_length =
    std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
  double s;
  if(vec_length == 0.0)
  {
    s = 0.0;
  }
  else
  {
    s = norm / vec_length;
  }

  std::vector<double> ret(3, 0.0);
  ret[0] = vec[0] * s;
  ret[1] = vec[1] * s;
  ret[2] = vec[2] * s;

  return ret;
}



void normalize_self(double norm, std::vector<double> &vec)
{
  double vec_length =
    std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
  double s;
  if(vec_length == 0.0)
  {
    s = 0.0;
  }
  else
  {
    s = norm / vec_length;
  }

  vec[0] *= s;
  vec[1] *= s;
  vec[2] *= s;
}



// not needed
/*
std::vector<double> normalize_to_max_coord(double max_coord,
                                           const std::vector<double> &vec)
{
  std::vector<double> ret(3, 0.0);

  double extreme_norm = std::abs(vec[0]);
  double el_1_abs = std::abs(vec[1]);
  if(el_1_abs > extreme_norm)
  {
    extreme_norm = el_1_abs;
  }
  double el_2_abs = std::abs(vec[2]);
  if(el_2_abs > extreme_norm)
  {
    extreme_norm = el_2_abs;
  }
  double s = max_coord / extreme_norm;

  ret[0] = vec[0] * s;
  ret[1] = vec[1] * s;
  ret[2] = vec[2] * s;

  std::cout << "\n\n";
  std::cout << "normalize_to_max_coord" << '\n';
  std::cout << "max_coord: " << max_coord << '\n';
  std::cout << "input: " <<
    vec[0] << ", " << vec[1] << ", " << vec[2] << '\n';
  std::cout << "s: " << s << '\n';
  std::cout << "output: " <<
    ret[0] << ", " << ret[1] << ", " << ret[2] << '\n';

  return ret;
}



void normalize_self_to_max_coord(double max_coord,
                                 std::vector<double> &vec)
{
  double extreme_norm = std::abs(vec[0]);
  double el_1_abs = std::abs(vec[1]);
  if(el_1_abs > extreme_norm)
  {
    extreme_norm = el_1_abs;
  }
  double el_2_abs = std::abs(vec[2]);
  if(el_2_abs > extreme_norm)
  {
    extreme_norm = el_2_abs;
  }
  double s = max_coord / extreme_norm;

  vec[0] *= s;
  vec[1] *= s;
  vec[2] *= s;
}
*/



std::vector<double> vector_plus(const std::vector<double> &first,
                                const std::vector<double> &second)
{
  std::vector<double> ret(3, 0.0);
  ret[0] = first[0] + second[0];
  ret[1] = first[1] + second[1];
  ret[2] = first[2] + second[2];
  return ret;
}



void vector_plus_self(std::vector<double> &first,
                      const std::vector<double> &second)
{
  first[0] += second[0];
  first[1] += second[1];
  first[2] += second[2];
}



std::vector<double> vector_minus(const std::vector<double> &first,
                                 const std::vector<double> &second)
{
  std::vector<double> ret(3, 0.0);
  ret[0] = first[0] - second[0];
  ret[1] = first[1] - second[1];
  ret[2] = first[2] - second[2];
  return ret;
}



void vector_minus_self(std::vector<double> &first,
                       const std::vector<double> &second)
{
  first[0] -= second[0];
  first[1] -= second[1];
  first[2] -= second[2];
}





wheel_data::wheel_data()
{
  steer = 0;
  r = std::vector<double>(3, 0.0);
  width = 0;
  radius = 0;
  bound_index = 0;
}

weapon_slot_data::weapon_slot_data()
{
  R_slots = std::vector<double>(3, 0.0);
  location_angle_of_slots = 0.0;
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
  for(std::size_t cur_coord = 0; cur_coord < 3; ++cur_coord)
  {
    point[cur_coord] *= scale_size;
  }
}



void vangers_model::scale_c3d(volInt::polyhedron &c3d_model)
{
  for(auto &&vert_to_change : c3d_model.verts)
  {
    scale_3d_point(vert_to_change);
  }

  c3d_model.set_xmax(c3d_model.xmax() * scale_size);
  c3d_model.set_ymax(c3d_model.ymax() * scale_size);
  c3d_model.set_zmax(c3d_model.zmax() * scale_size);

  c3d_model.set_xmin(c3d_model.xmin() * scale_size);
  c3d_model.set_ymin(c3d_model.ymin() * scale_size);
  c3d_model.set_zmin(c3d_model.zmin() * scale_size);

  c3d_model.set_x_off(c3d_model.x_off() * scale_size);
  c3d_model.set_y_off(c3d_model.y_off() * scale_size);
  c3d_model.set_z_off(c3d_model.z_off() * scale_size);

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
