#ifndef TRACTOR_CONVERTER_VANGERS_3D_MODEL_OPERATIONS_H
#define TRACTOR_CONVERTER_VANGERS_3D_MODEL_OPERATIONS_H

#include "defines.hpp"
#include "wavefront_obj_constants.hpp"
#include "vangers_3d_model_constants.hpp"

#include "raw_num_operations.hpp"
#include "file_operations.hpp"
#include "to_string_precision.hpp"
#include "vangers_cfg_operations.hpp"
//#include "wavefront_obj_operations.hpp"

#include "volInt.hpp"
#include "tiny_obj_loader.h"

#include <boost/filesystem.hpp>
//#include <boost/filesystem/fstream.hpp>

#include <exception>
#include <stdexcept>

#include <cmath>
#include <algorithm>
#include <utility>
#include <iterator>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace tractor_converter{
namespace helpers{



typedef std::vector<double> point;



/*
// VANGERS SOURCE
// how angle conversions works in vangers code
#define M_PI      3.14159265358979323846

Pi_len = 11
const int  Pi    = 1 << Pi_len;
2048

#define GTOR(x) (double(x)*(M_PI/double(Pi)))
#define RTOG(x) (round(x*(double(Pi)/M_PI)))
*/

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif
//const double M_PI = 3.14159265358979323846;
const int sicher_angle_Pi = 2048; // "1 << Pi_len" where "Pi_len = 11".

enum rotation_axis{x, y, z};



const int sprintf_float_per_file_cfg_format_precision = 6;

const std::string sprintf_float_per_file_cfg_format =
  "%." + std::to_string(sprintf_float_per_file_cfg_format_precision) + "g";



double sicher_angle_to_radians(int sicher_angle);
int radians_to_sicher_angle(double radians);

void rotate_3d_point_by_axis(
  std::vector<double> &point,
  double angle_sin,
  double angle_cos,
  rotation_axis axis);
void rotate_3d_point_by_axis(
  std::vector<double> &point,
  double angle,
  rotation_axis axis);
void rotate_c3d_by_axis(
  volInt::polyhedron &c3d_model,
  double angle,
  rotation_axis axis);



const std::vector<std::string> vangers_3d_tree_folders
{
  "ammun",
  "animated",
  "fauna",
  "items",
  "mechous",
  "unique",
  "weapon",
};

struct vangers_3d_paths_game_dir
{
  struct io_paths
  {
    boost::filesystem::path input;
    boost::filesystem::path output;
  };

  io_paths root;
  io_paths game_lst;

  std::unordered_map<std::string, io_paths> mechous_prm;
  std::unordered_map<std::string, io_paths> mechous_m3d;
  std::unordered_map<std::string, io_paths> weapon_m3d;
  std::unordered_map<std::string, io_paths> animated_a3d;
  std::unordered_map<std::string, io_paths> other_m3d;
};



struct wheel_data
{
  wheel_data();

  int steer;
  std::vector<double> r;
  double width;
  double radius;
  int bound_index;

  // Does not exist in actual *.m3d file.
  int ghost;
  std::size_t wheel_model_index;
};

struct weapon_slot_data
{
  weapon_slot_data();

  std::vector<double> R_slots;
  double location_angle_of_slots;

  bool exists;
};



class vangers_model
{
public:

//enum struct file_type{m3d, a3d, none};

  vangers_model(
    const boost::filesystem::path &input_file_path_arg,
    const boost::filesystem::path &output_dir_path_arg,
    const std::string &input_file_name_error_arg,
    const std::string &output_file_name_error_arg,
    const volInt::polyhedron *example_weapon_model_arg = nullptr,
    const volInt::polyhedron *weapon_attachment_point_arg = nullptr,
    const volInt::polyhedron *ghost_wheel_model_arg = nullptr,
    const volInt::polyhedron *center_of_mass_model_arg = nullptr);

protected:

  std::string model_name;

  std::string m3d_data;
  std::size_t m3d_data_cur_pos;

  const boost::filesystem::path input_file_path;
  const boost::filesystem::path output_dir_path;
  const std::string input_file_name_error;
  const std::string output_file_name_error;


  double scale_size;



  int n_wheels;
  int n_debris;
  int n_models;

  volInt::model_extreme_points extreme_points;
  double rmax;

  int body_color_offset;
  int body_color_shift;

  std::vector<wheel_data> cur_wheel_data;

  int weapon_slots_existence;
  std::vector<weapon_slot_data> cur_weapon_slot_data;
  const volInt::polyhedron *example_weapon_model;
  const volInt::polyhedron *weapon_attachment_point;
  const volInt::polyhedron *ghost_wheel_model;
  const volInt::polyhedron *center_of_mass_model;


  std::pair<std::vector<double>, std::vector<double>> &extreme_points_pair();
  const std::pair<std::vector<double>, std::vector<double>> &
    extreme_points_pair() const;
  std::vector<double> &max_point();
  const std::vector<double> &max_point() const;
  std::vector<double> &min_point();
  const std::vector<double> &min_point() const;

  double xmax() const;
  double ymax() const;
  double zmax() const;

  double xmin() const;
  double ymin() const;
  double zmin() const;

  void set_xmax(double new_xmax);
  void set_ymax(double new_ymax);
  void set_zmax(double new_zmax);

  void set_xmin(double new_xmin);
  void set_ymin(double new_ymin);
  void set_zmin(double new_zmin);

// not needed
//void scale_3d_point(std::vector<double> &point);
//void scale_c3d(volInt::polyhedron &c3d_model);
};



} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_VANGERS_3D_MODEL_OPERATIONS_H
