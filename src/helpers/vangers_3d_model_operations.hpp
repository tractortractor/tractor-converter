#ifndef TRACTOR_CONVERTER_VANGERS_3D_MODEL_OPERATIONS_H
#define TRACTOR_CONVERTER_VANGERS_3D_MODEL_OPERATIONS_H

#include "defines.hpp"
#include "bitflag.hpp"
#include "wavefront_obj_constants.hpp"
#include "vangers_3d_model_constants.hpp"

#include "raw_num_operations.hpp"
#include "file_operations.hpp"
#include "to_string_precision.hpp"
#include "vangers_cfg_operations.hpp"

#include "volInt.hpp"
#include "tiny_obj_loader.h"

#include <boost/filesystem.hpp>

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



enum class normal_flag
{
  none = 0,
  sort_info = 1,
};

typedef long double scaled_float;
typedef std::vector<double> point;



const int sprintf_float_per_file_cfg_format_precision = 6;

const std::string sprintf_float_per_file_cfg_format =
  "%." + std::to_string(sprintf_float_per_file_cfg_format_precision) + "g";



const std::unordered_set<std::string> vangers_3d_tree_folders
{
  folder::ammun,
  folder::animated,
  folder::fauna,
  folder::items,
  folder::mechous,
  folder::unique,
  folder::weapon,
};



enum class merge_model_type{wheel, weapon, attachment_point, center_of_mass};
enum class merge_with_weapons_flag
{
  none = 0,
  extract_nonexistent_weapons = 1,
};



template<typename SOURCE, typename DESTINATION>
std::vector<DESTINATION> cast_vec_var(
  const std::vector<SOURCE> &vec_src)
{
  return std::vector<DESTINATION>(vec_src.begin(), vec_src.end());
}



template<typename SOURCE, typename DESTINATION>
std::vector<std::vector<DESTINATION>> cast_nest_vec_var(
  const std::vector<std::vector<SOURCE>> &nest_vec_src)
{
  std::vector<std::vector<DESTINATION>> nest_vec_dest;
  std::transform(
    nest_vec_src.begin(), nest_vec_src.end(),
    std::back_inserter(nest_vec_dest),
    [](const std::vector<SOURCE> &vec_src)
      {
        return cast_vec_var<SOURCE, DESTINATION>(vec_src);
      }
  );
  return nest_vec_dest;
}



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

  // Those don't exist in actual *.m3d file.
  int ghost;
  std::size_t wheel_model_index;
};

struct weapon_slot_data
{
  weapon_slot_data();

  std::vector<double> R_slot;
  double location_angle_of_slot;

  bool exists;
};



class vangers_model
{
public:

  vangers_model(
    const boost::filesystem::path &input_m3d_path_arg,
    const boost::filesystem::path &output_m3d_path_arg,
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

  const boost::filesystem::path input_m3d_path;
  const boost::filesystem::path output_m3d_path;
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



  std::string file_prefix_to_filename(const std::string &prefix,
                                      const std::size_t *model_num = nullptr);

  template<typename WHEELS_CONTAINER>
  void merge_helper_reserve_space_in_main(
    volInt::polyhedron *main_model,
    const WHEELS_CONTAINER *steer_wheels_models = nullptr,
    const WHEELS_CONTAINER *non_steer_ghost_wheels_models = nullptr) const
  {
    std::size_t new_main_model_num_verts = main_model->numVerts;
    std::size_t new_main_model_num_vert_norms = main_model->numVertNorms;
    std::size_t new_main_model_num_poly = main_model->numFaces;
    if(n_wheels)
    {
      for(std::size_t cur_wheel_num = 0;
          cur_wheel_num < cur_wheel_data.size();
          ++cur_wheel_num)
      {
        if(steer_wheels_models &&
           cur_wheel_data[cur_wheel_num].steer)
        {
          const volInt::polyhedron &wheel_model =
            (*steer_wheels_models).at(
              cur_wheel_data[cur_wheel_num].wheel_model_index);
          new_main_model_num_verts += wheel_model.numVerts;
          new_main_model_num_vert_norms += wheel_model.numVertNorms;
          new_main_model_num_poly += wheel_model.numFaces;
        }
        else if(non_steer_ghost_wheels_models &&
                cur_wheel_data[cur_wheel_num].ghost)
        {
          const volInt::polyhedron &wheel_model =
            (*non_steer_ghost_wheels_models).at(
              cur_wheel_data[cur_wheel_num].wheel_model_index);
          new_main_model_num_verts += wheel_model.numVerts;
          new_main_model_num_vert_norms += wheel_model.numVertNorms;
          new_main_model_num_poly += wheel_model.numFaces;
        }
      }
    }
    // Example weapon models are no longer merged with main model.
    // Center of mass model is no longer merged with main model.
    // Memory is not reserved for those.
    main_model->verts.reserve(new_main_model_num_verts);
    main_model->vertNorms.reserve(new_main_model_num_vert_norms);
    main_model->faces.reserve(new_main_model_num_poly);
  }

  void merge_helper_move_model_into_main(volInt::polyhedron &main_model,
                                         volInt::polyhedron &model_to_move,
                                         std::vector<double> new_position,
                                         double new_angle,
                                         int wheel_id,
                                         int weapon_id,
                                         merge_model_type merge_type) const;

  template<typename WHEELS_CONTAINER>
  void merge_main_model_with_wheels(
    volInt::polyhedron *main_model,
    WHEELS_CONTAINER *steer_wheels_models = nullptr,
    WHEELS_CONTAINER *non_steer_ghost_wheels_models = nullptr) const
  {
    // Inserting steering wheels into main model.
    for(std::size_t cur_wheel_num = 0;
        cur_wheel_num < cur_wheel_data.size();
        ++cur_wheel_num)
    {
      if(steer_wheels_models &&
         cur_wheel_data[cur_wheel_num].steer)
      {
        merge_helper_move_model_into_main(
          *main_model,
          (*steer_wheels_models)
            [cur_wheel_data[cur_wheel_num].wheel_model_index],
          cur_wheel_data[cur_wheel_num].r,
          0.0,
          cur_wheel_num,
          volInt::invalid::weapon_id,
          merge_model_type::wheel);
      }
      else if(non_steer_ghost_wheels_models &&
              cur_wheel_data[cur_wheel_num].ghost)
      {
        merge_helper_move_model_into_main(
          *main_model,
          (*non_steer_ghost_wheels_models)
            [cur_wheel_data[cur_wheel_num].wheel_model_index],
          cur_wheel_data[cur_wheel_num].r,
          0.0,
          cur_wheel_num,
          volInt::invalid::weapon_id,
          merge_model_type::wheel);
      }
    }
  }

  void merge_model_with_weapon_attachment_point(
    volInt::polyhedron &main_model) const;

  volInt::model_extreme_points get_wheel_params_extremes();



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



  // Returning long double in case T is long double.
  template <typename T>
  scaled_float scale(T var, double exp = 1.0)
  {
    return var * std::pow(scale_size, exp);
  }

  // If T is integer, the result will be truncated.
  template <typename T>
  T scale_trunc(T var, double exp = 1.0)
  {
    return var * std::pow(scale_size, exp);
  }


  template <typename T>
  std::vector<scaled_float> scale_vec(
    const std::vector<T> &vec, double exp = 1.0)
  {
    std::vector<scaled_float> dest_vec;
    std::transform(vec.begin(), vec.end(),
                   std::back_inserter(dest_vec),
                   [&](T var){ return scale<T>(var, exp); });
    return dest_vec;
  }

  template <typename T>
  void scale_vec_trunc(
    std::vector<T> &vec, double exp = 1.0)
  {
    std::transform(vec.begin(), vec.end(),
                   vec.begin(),
                   [&](T var){ return scale_trunc<T>(var, exp); });
  }


  template <typename T>
  std::vector<std::vector<scaled_float>> scale_nest_vec(
    const std::vector<std::vector<T>> &nest_vec, double exp = 1.0)
  {
    std::vector<std::vector<scaled_float>> dest_nest_vec;
    std::transform(
      nest_vec.begin(), nest_vec.end(),
      std::back_inserter(dest_nest_vec),
      [&](const std::vector<T> &vec){ return scale_vec<T>(vec, exp); });
    return dest_nest_vec;
  }

  template <typename T>
  void scale_nest_vec_trunc(
    std::vector<std::vector<T>> &nest_vec, double exp = 1.0)
  {
    std::for_each(
      nest_vec.begin(), nest_vec.end(),
      [&](std::vector<T> &vec){ scale_vec_trunc<T>(vec, exp); });
  }
};



} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_VANGERS_3D_MODEL_OPERATIONS_H
