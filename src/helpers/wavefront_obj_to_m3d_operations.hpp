#ifndef TRACTOR_CONVERTER_WAVEFRONT_OBJ_TO_M3D_OPERATIONS_H
#define TRACTOR_CONVERTER_WAVEFRONT_OBJ_TO_M3D_OPERATIONS_H

#include "defines.hpp"
#include "bitflag.hpp"
#include "wavefront_obj_constants.hpp"
#include "vangers_3d_model_constants.hpp"

#include "raw_num_operations.hpp"
#include "file_operations.hpp"
#include "to_string_precision.hpp"
#include "vangers_cfg_operations.hpp"
#include "wavefront_obj_operations.hpp"
#include "vangers_3d_model_operations.hpp"

#include "volInt.hpp"
#include "tiny_obj_loader.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
//#include <boost/filesystem/fstream.hpp>

#include <exception>
#include <stdexcept>

#include <cstring>
#include <cmath>
#include <cctype>
#include <iostream>
#include <algorithm>
#include <utility>
#include <limits>
#include <iterator>
#include <string>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace tractor_converter{
namespace helpers{



namespace exception{
  struct model_ref_points_not_found: public virtual std::runtime_error
  {
    using std::runtime_error::runtime_error;
  };
} // namespace exception



enum class obj_to_m3d_flag
{
  none = 0,
  center_model = 1,
  recalculate_vertex_normals = 2,
  generate_bound_models = 3,
};

const std::size_t J_cfg_num_of_values = 9;



// 1 digit left to dot + 1 dot + 8 expected digits right to dot + 4 exponent
const std::size_t sicher_prm_float_size_increase = 1 + 1 + 8 + 4;
// 19 integer number
const std::size_t sicher_game_lst_int_size_increase = 19;



template <typename FLOAT>
FLOAT round_away_from_zero(FLOAT num)
{
  return num < 0 ? std::floor(num) : std::ceil(num);
//  return std::copysign(std::ceil(std::fabs(num)), num);
}



const double round_half_to_even_remainder_max =
  0.5 + 1.0/std::pow(10, volInt::min_float_precision); // 0.501
const double round_half_to_even_remainder_min =
  0.5 - 1.0/std::pow(10, volInt::min_float_precision); // 0.499

template <typename FLOAT, typename INT>
INT round_half_to_even(FLOAT num)
{
  INT num_round = std::round(num);
  INT num_trunc = std::trunc(num);
  FLOAT remainder = num - num_round;
  FLOAT remainder_fabs = std::fabs(remainder);

  if(remainder_fabs < round_half_to_even_remainder_max &&
     remainder_fabs > round_half_to_even_remainder_min)
  {
    // if odd
    if(num_trunc % 2)
    {
      return round_away_from_zero<FLOAT>(num);
    }
    // if even
    else
    {
      return num_trunc;
    }
  }
  else
  {
    return num_round;
  }
}



template <typename FLOAT, typename INT>
std::vector<INT> round_half_to_even_vec(const std::vector<FLOAT> &vec)
{
  std::vector<INT> dest_vec;
  std::transform(vec.begin(), vec.end(),
                 std::back_inserter(dest_vec),
                 round_half_to_even<FLOAT, INT>);
  return dest_vec;
}



template <typename FLOAT, typename INT>
std::vector<std::vector<INT>> round_half_to_even_nest_vec(
  const std::vector<std::vector<FLOAT>> &nest_vec)
{
  std::vector<std::vector<INT>> dest_nest_vec;
  std::transform(nest_vec.begin(), nest_vec.end(),
                 std::back_inserter(dest_nest_vec),
                 round_half_to_even_vec<FLOAT, INT>);
  return dest_nest_vec;
}



void get_extreme_radius(double &cur_extreme,
                        double radius,
                        point offset = {0.0, 0.0, 0.0});



// Get radius of bound sphere around attachment point.
double get_weapons_bound_sphere_radius(
  const std::unordered_map<std::string, volInt::polyhedron> &weapons_models);



template<class T>
T parse_per_file_cfg_option(
  const std::string &input);

template<class T>
std::vector<T> parse_per_file_cfg_multiple_options(
  const std::string &input);

template<class T>
std::vector<T> parse_per_file_cfg_multiple_options(
  const std::vector<std::string> &input);



class wavefront_obj_to_m3d_model : vangers_model
{
public:

  wavefront_obj_to_m3d_model(
    const boost::filesystem::path &input_m3d_path_arg,
    const boost::filesystem::path &output_m3d_path_arg,
    const std::string &input_file_name_error_arg,
    const std::string &output_file_name_error_arg,
    const volInt::polyhedron *example_weapon_model_arg,
    const volInt::polyhedron *weapon_attachment_point_arg,
    const volInt::polyhedron *center_of_mass_model_arg,
    double max_weapons_radius_arg,
    unsigned int c3d_default_material_id_arg,
    double scale_cap_arg,
    double max_smooth_angle_arg,
    std::size_t gen_bound_layers_num_arg,
    double gen_bound_area_threshold_arg,
    bitflag<obj_to_m3d_flag> flags_arg,
    std::unordered_map<std::string, double> *non_mechos_scale_sizes_arg);

  void mechos_wavefront_objs_to_m3d();
  volInt::polyhedron weapon_wavefront_objs_to_m3d();
  void animated_wavefront_objs_to_a3d();
  void other_wavefront_objs_to_m3d();

private:

  const double max_weapons_radius;
  unsigned int c3d_default_material_id;
  double scale_cap;
  double max_smooth_angle;
  std::size_t gen_bound_layers_num;
  double gen_bound_area_threshold;
  bitflag<obj_to_m3d_flag> flags;
  std::unordered_map<std::string, double> *non_mechos_scale_sizes;
  double prm_scale_size;


  boost::filesystem::path file_prefix_to_path(
    const std::string &prefix,
    const std::size_t *model_num = nullptr);


  void read_file_cfg_helper_overwrite_volume(
    volInt::polyhedron &model,
    const double custom_volume);
  void read_file_cfg_helper_overwrite_J(
    volInt::polyhedron &model,
    const std::vector<double> &custom_J);
  void read_file_cfg_m3d(
    volInt::polyhedron &main_model,
    std::deque<volInt::polyhedron> *debris_models = nullptr);
  void read_file_cfg_a3d(std::deque<volInt::polyhedron> &animated_models);

  volInt::polyhedron read_obj(
    const boost::filesystem::path &obj_input_file_path,
    c3d::c3d_type cur_c3d_type);
  volInt::polyhedron read_obj_prefix(
    const std::string &prefix,
    c3d::c3d_type cur_c3d_type);
  std::deque<volInt::polyhedron> read_objs_with_prefix(
    const std::string &prefix,
    c3d::c3d_type cur_c3d_type);



  template<typename SOURCE, typename DESTINATION>
  void write_var_to_m3d(SOURCE var)
  {
    num_to_raw_bytes<DESTINATION>(var, m3d_data, m3d_data_cur_pos);
    m3d_data_cur_pos += sizeof(DESTINATION);
  }

  template<typename SOURCE, typename DESTINATION>
  void write_var_to_m3d_rounded(SOURCE var)
  {
    write_var_to_m3d<DESTINATION, DESTINATION>(
      round_half_to_even<SOURCE, DESTINATION>(var));
  }

  template<typename SOURCE, typename DESTINATION>
  void write_var_to_m3d_scaled(SOURCE var, double exp = 1.0)
  {
    write_var_to_m3d<SOURCE, DESTINATION>(scale_trunc<SOURCE>(var, exp));
  }

  template<typename SOURCE, typename DESTINATION>
  void write_var_to_m3d_scaled_rounded(SOURCE var, double exp = 1.0)
  {
    write_var_to_m3d_rounded<scaled_float, DESTINATION>(
      scale<SOURCE>(var, exp));
  }


  template<typename SOURCE, typename DESTINATION>
  void write_vec_var_to_m3d(const std::vector<SOURCE> &vec_src)
  {
    std::vector<DESTINATION> vec_dest =
      cast_vec_var<SOURCE, DESTINATION>(vec_src);
    vec_num_to_raw_bytes<DESTINATION>(vec_dest, m3d_data, m3d_data_cur_pos);
    m3d_data_cur_pos += sizeof(DESTINATION) * vec_dest.size();
  }

  template<typename SOURCE, typename DESTINATION>
  void write_vec_var_to_m3d_rounded(const std::vector<SOURCE> &vec)
  {
    write_vec_var_to_m3d<DESTINATION, DESTINATION>(
      round_half_to_even_vec<SOURCE, DESTINATION>(vec));
  }

  template<typename SOURCE, typename DESTINATION>
  void write_vec_var_to_m3d_scaled(std::vector<SOURCE> vec, double exp = 1.0)
  {
    scale_vec_trunc<SOURCE>(vec, exp);
    write_vec_var_to_m3d<SOURCE, DESTINATION>(vec);
  }

  template<typename SOURCE, typename DESTINATION>
  void write_vec_var_to_m3d_scaled_rounded(
    const std::vector<SOURCE> &vec, double exp = 1.0)
  {
    write_vec_var_to_m3d_rounded<scaled_float, DESTINATION>(
      scale_vec<SOURCE>(vec, exp));
  }


  template<typename SOURCE, typename DESTINATION>
  void write_nest_vec_var_to_m3d(
    const std::vector<std::vector<SOURCE>> &nest_vec_src)
  {
    std::vector<std::vector<DESTINATION>> nest_vec_dest =
      cast_nest_vec_var<SOURCE, DESTINATION>(nest_vec_src);
    nest_vec_num_to_raw_bytes<DESTINATION>(nest_vec_dest,
                                           m3d_data,
                                           m3d_data_cur_pos);
    for(const auto &vec_dest : nest_vec_dest)
    {
      m3d_data_cur_pos += sizeof(DESTINATION) * vec_dest.size();
    }
  }

  template<typename SOURCE, typename DESTINATION>
  void write_nest_vec_var_to_m3d_rounded(
    const std::vector<std::vector<SOURCE>> &nest_vec)
  {
    write_nest_vec_var_to_m3d<DESTINATION, DESTINATION>(
      round_half_to_even_nest_vec<SOURCE, DESTINATION>(nest_vec));
  }

  template<typename SOURCE, typename DESTINATION>
  void write_nest_vec_var_to_m3d_scaled(
    std::vector<std::vector<SOURCE>> nest_vec, double exp = 1.0)
  {
    scale_nest_vec_trunc<SOURCE>(nest_vec, exp);
    write_nest_vec_var_to_m3d<SOURCE, DESTINATION>(nest_vec);
  }

  template<typename SOURCE, typename DESTINATION>
  void write_nest_vec_var_to_m3d_scaled_rounded(
    const std::vector<std::vector<SOURCE>> &nest_vec, double exp = 1.0)
  {
    write_nest_vec_var_to_m3d_rounded<scaled_float, DESTINATION>(
      scale_nest_vec<SOURCE>(nest_vec, exp));
  }



  std::vector<double> get_medium_vert(const volInt::polyhedron &model,
                                      const volInt::face &poly);

  void write_vertex(const std::vector<double> &vert);
  void write_vertices(const volInt::polyhedron &model);

  void write_normal(const std::vector<double> &norm,
                    bitflag<normal_flag> flags);
  void write_normals(const volInt::polyhedron &model);

  void write_polygon(const volInt::polyhedron &model,
                     const volInt::face &poly);
  void write_polygons(const volInt::polyhedron &model);

  void write_sorted_polygon_indices(const volInt::polyhedron &model);

  void write_c3d(const volInt::polyhedron &model);

  void write_m3d_header_data();

  void write_m3d_wheel_data(
    std::unordered_map<int, volInt::polyhedron> &wheels_models,
    std::size_t wheel_id);
  void write_m3d_wheels_data(
    std::unordered_map<int, volInt::polyhedron> &wheels_models);

  void write_m3d_debris_data(
    volInt::polyhedron &debris_model,
    volInt::polyhedron &debris_bound_model);
  void write_m3d_debris_data(
    std::deque<volInt::polyhedron> &debris_models,
    std::deque<volInt::polyhedron> &debris_bound_models);

  void write_m3d_weapon_slot(std::size_t slot_id);
  void write_m3d_weapon_slots();

  void write_a3d_header_data();


  void for_each_steer_non_ghost_wheel(
    const volInt::polyhedron *main_model,
    const std::unordered_map<int, volInt::polyhedron> *wheels_models,
    std::function<void(const volInt::polyhedron&)> func_to_call);
  void for_each_steer_non_ghost_wheel(
    const volInt::polyhedron *main_model,
    std::unordered_map<int, volInt::polyhedron> *wheels_models,
    std::function<void(volInt::polyhedron&)> func_to_call);


  std::vector<point*> get_ref_points_for_part_of_model(
    volInt::polyhedron &model,
    const volInt::polyhedron *reference_model,
    unsigned int color_id,
    int wheel_weapon_id = -1);

  std::pair<point, point> get_compare_points(
    std::vector<point*> cur_ref_verts,
    const volInt::polyhedron *reference_model);
  std::pair<point, point> get_compare_points(
    std::vector<point*> cur_ref_verts,
    std::vector<point*> ref_model_ref_verts);

  void get_custom_rcm(volInt::polyhedron &main_model);

  void get_attachment_point(volInt::polyhedron &main_model);

  void get_weapons_data(volInt::polyhedron &main_model);

  std::unordered_map<int, volInt::polyhedron> get_wheels_steer(
    volInt::polyhedron &main_model);



  void get_m3d_extreme_points(
    const volInt::polyhedron *main_model,
    const std::unordered_map<int, volInt::polyhedron> *wheels_models =
      nullptr);
  void get_m3d_extreme_points_calc_c3d_extr(
    volInt::polyhedron *main_model,
    std::unordered_map<int, volInt::polyhedron> *wheels_models =
      nullptr);
  void get_a3d_extreme_points(
    const std::deque<volInt::polyhedron> *models);
  void get_a3d_extreme_points_calc_c3d_extr(
    std::deque<volInt::polyhedron> *models);



  void get_m3d_header_data(
    volInt::polyhedron *main_model,
    volInt::polyhedron *main_bound_model = nullptr,
    std::unordered_map<int, volInt::polyhedron> *wheels_models = nullptr,
    std::deque<volInt::polyhedron> *debris_models = nullptr,
    std::deque<volInt::polyhedron> *debris_bound_models = nullptr);
  void get_a3d_header_data(std::deque<volInt::polyhedron> *models);


  void get_wheels_data(const volInt::polyhedron &main_model);

  void get_debris_data(
    const std::deque<volInt::polyhedron> *debris_models,
    const std::deque<volInt::polyhedron> *debris_bound_models = nullptr);

  void center_debris(
    std::deque<volInt::polyhedron> *debris_models,
    std::deque<volInt::polyhedron> *debris_bound_models = nullptr);
  void center_debris(
    volInt::polyhedron *debris_model,
    volInt::polyhedron *debris_bound_model = nullptr);

  void center_m3d(
    volInt::polyhedron *main_model,
    volInt::polyhedron *main_bound_model = nullptr,
    std::unordered_map<int, volInt::polyhedron> *wheels_models = nullptr,
    std::deque<volInt::polyhedron> *debris_models = nullptr,
    std::deque<volInt::polyhedron> *debris_bound_models = nullptr);
  void center_a3d(std::deque<volInt::polyhedron> *models);

  void get_scale_helper_get_extreme_radius(
    volInt::polyhedron *model,
    double &extreme_radius,
    const point offset = {0.0, 0.0, 0.0});
  void get_scale_helper_set_scale_from_rmax();
  void get_m3d_scale_size(
    volInt::polyhedron *main_model,
    volInt::polyhedron *main_bound_model = nullptr,
    std::unordered_map<int, volInt::polyhedron> *wheels_models = nullptr,
    std::deque<volInt::polyhedron> *debris_models = nullptr,
    std::deque<volInt::polyhedron> *debris_bound_models = nullptr);
  void get_a3d_scale_size(std::deque<volInt::polyhedron> *models);

  void m3d_mechos_generate_bound(
    const volInt::polyhedron *main_model,
    const std::unordered_map<int, volInt::polyhedron> *wheels_models,
    const std::deque<volInt::polyhedron> *debris_models,
    volInt::polyhedron &new_main_bound,
    std::deque<volInt::polyhedron> &new_debris_bounds);
  void m3d_non_mechos_generate_bound(
    const volInt::polyhedron *main_model,
    volInt::polyhedron &new_main_bound);

  void m3d_recalc_vertNorms(
    volInt::polyhedron *main_model,
    volInt::polyhedron *main_bound_model = nullptr,
    std::unordered_map<int, volInt::polyhedron> *wheels_models = nullptr,
    std::deque<volInt::polyhedron> *debris_models = nullptr,
    std::deque<volInt::polyhedron> *debris_bound_models = nullptr);
  void a3d_recalc_vertNorms(std::deque<volInt::polyhedron> *models);

  std::size_t get_c3d_file_size(const volInt::polyhedron *model);
  std::size_t get_m3d_file_size(
    const volInt::polyhedron *main_model,
    const volInt::polyhedron *main_bound_model = nullptr,
    const std::unordered_map<int, volInt::polyhedron> *wheels_models = nullptr,
    const std::deque<volInt::polyhedron> *debris_models = nullptr,
    const std::deque<volInt::polyhedron> *debris_bound_models = nullptr);
  std::size_t get_a3d_file_size(const std::deque<volInt::polyhedron> *models);

  enum class remove_polygons_model{mechos, weapon, regular};
  void remove_polygons_helper_erase_mechos(volInt::polyhedron &main_model);
  void remove_polygons_helper_erase_weapons(volInt::polyhedron &main_model);
  void remove_polygons_helper_erase_regular(volInt::polyhedron &model);
  std::vector<std::size_t> remove_polygons_helper_create_ind_change_map(
    std::size_t size,
    std::unordered_set<std::size_t> &verts_to_keep);
  void remove_polygons(volInt::polyhedron &main_model,
                       remove_polygons_model model_type);
};



void create_game_lst(
  const boost::filesystem::path &input_file_path_arg,
  const boost::filesystem::path &where_to_save_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const std::unordered_map<std::string, double> *non_mechos_scale_sizes_arg);

void create_prm(
  const boost::filesystem::path &input_file_path_arg,
  const boost::filesystem::path &where_to_save_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const double scale_size);

void mechos_wavefront_objs_to_m3d(
  const boost::filesystem::path &input_m3d_path_arg,
  const boost::filesystem::path &output_m3d_path_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const volInt::polyhedron *example_weapon_model_arg,
  const volInt::polyhedron *weapon_attachment_point_model_arg,
  const volInt::polyhedron *center_of_mass_model_arg,
  double max_weapons_radius_arg,
  unsigned int c3d_default_material_id_arg,
  double scale_cap_arg,
  double max_smooth_angle_arg,
  std::size_t gen_bound_layers_num_arg,
  double gen_bound_area_threshold_arg,
  bitflag<obj_to_m3d_flag> flags_arg);

volInt::polyhedron weapon_wavefront_objs_to_m3d(
  const boost::filesystem::path &input_m3d_path_arg,
  const boost::filesystem::path &output_m3d_path_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const volInt::polyhedron *weapon_attachment_point_arg,
  const volInt::polyhedron *center_of_mass_model_arg,
  unsigned int c3d_default_material_id_arg,
  double scale_cap_arg,
  double max_smooth_angle_arg,
  std::size_t gen_bound_layers_num_arg,
  double gen_bound_area_threshold_arg,
  bitflag<obj_to_m3d_flag> flags_arg,
  std::unordered_map<std::string, double> *non_mechos_scale_sizes_arg);

void animated_wavefront_objs_to_a3d(
  const boost::filesystem::path &input_m3d_path_arg,
  const boost::filesystem::path &output_m3d_path_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const volInt::polyhedron *center_of_mass_model_arg,
  unsigned int c3d_default_material_id_arg,
  double scale_cap_arg,
  double max_smooth_angle_arg,
  bitflag<obj_to_m3d_flag> flags_arg,
  std::unordered_map<std::string, double> *non_mechos_scale_sizes_arg);

void other_wavefront_objs_to_m3d(
  const boost::filesystem::path &input_m3d_path_arg,
  const boost::filesystem::path &output_m3d_path_arg,
  const std::string &input_file_name_error_arg,
  const std::string &output_file_name_error_arg,
  const volInt::polyhedron *center_of_mass_model_arg,
  unsigned int c3d_default_material_id_arg,
  double scale_cap_arg,
  double max_smooth_angle_arg,
  std::size_t gen_bound_layers_num_arg,
  double gen_bound_area_threshold_arg,
  bitflag<obj_to_m3d_flag> flags_arg,
  std::unordered_map<std::string, double> *non_mechos_scale_sizes_arg);

} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_WAVEFRONT_OBJ_TO_M3D_OPERATIONS_H
