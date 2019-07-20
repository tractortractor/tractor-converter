#ifndef TRACTOR_CONVERTER_VANGERS_MODELS_CONSTANTS_H
#define TRACTOR_CONVERTER_VANGERS_MODELS_CONSTANTS_H

#include "defines.hpp"

#include "volInt.hpp"

//#include <boost/bimap.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/tags/tagged.hpp>
#include <boost/assign.hpp>

#include <string>
#include <unordered_map>

namespace tractor_converter{

namespace c3d{

  const int scaling_max_extreme_radius = 123;
  const double vector_scale_val = 123.0;

  enum class c3d_type
  {
    regular,
    bound,
  };

  const int version_req = 8;

  const std::vector<int> default_phi_psi_tetta = std::vector<int>(3, 83);

  const std::size_t regular_model_vertices_per_polygon = 3;
  const std::size_t bound_model_vertices_per_polygon = 4;



  namespace header{
    // 0  int
    const std::size_t version_pos = 0;
    // 4  int
    const std::size_t num_vert_pos = version_pos + sizeof(int);
    // 8  int
    const std::size_t num_norm_pos = num_vert_pos + sizeof(int);
    // 12  int
    const std::size_t num_poly_pos = num_norm_pos + sizeof(int);
    // 16  int
    const std::size_t num_vert_total_pos = num_poly_pos + sizeof(int);



    // 20  int
    const std::size_t xmax_pos = num_vert_total_pos + sizeof(int);
    // 24  int
    const std::size_t ymax_pos = xmax_pos + sizeof(int);
    // 28  int
    const std::size_t zmax_pos = ymax_pos + sizeof(int);

    // 32  int
    const std::size_t xmin_pos = zmax_pos + sizeof(int);
    // 36  int
    const std::size_t ymin_pos = xmin_pos + sizeof(int);
    // 40  int
    const std::size_t zmin_pos = ymin_pos + sizeof(int);

    // 44  int
    const std::size_t x_off_pos = zmin_pos + sizeof(int);
    // 48  int
    const std::size_t y_off_pos = x_off_pos + sizeof(int);
    // 52  int
    const std::size_t z_off_pos = y_off_pos + sizeof(int);

    // 56  int
    const std::size_t rmax_pos = z_off_pos + sizeof(int);



    // 60  int
    const std::size_t phi_pos = rmax_pos + sizeof(int);
    // 64  int
    const std::size_t psi_pos = phi_pos + sizeof(int);
    // 68  int
    const std::size_t tetta_pos = psi_pos + sizeof(int);


    // 72   double
    const std::size_t volume_pos = tetta_pos + sizeof(int);
    // 80  double vector
    const std::size_t rcm_pos = volume_pos + sizeof(double);
    // 104  double matrix
    const std::size_t J_pos = rcm_pos + sizeof(double)*volInt::axes_num;



    // 176
    const std::size_t size =
      J_pos + sizeof(double)*volInt::axes_num*volInt::axes_num;
  } // namespace header

  const std::size_t data_pos = header::size;



  namespace vertex{
    const int default_sort_info = 0;

    // 4  float
    const std::size_t full_coord_size = sizeof(float);
    // 12
    const std::size_t full_coords_size =
      full_coord_size * volInt::axes_num;

    // 1  char
    const std::size_t compact_coord_size = sizeof(char);
    // 3
    const std::size_t compact_coords_size =
      compact_coord_size * volInt::axes_num;

    // 4  int
    const std::size_t sort_info_size = sizeof(int);



    // 7
    const std::size_t size_no_full_coords = compact_coords_size +
                                            sort_info_size;

    // 19
    const std::size_t size = full_coords_size +
                             compact_coords_size +
                             sort_info_size;



    // 0
    const std::size_t full_coord_x_rel_pos = full_coord_size * 0;
    // 4
    const std::size_t full_coord_y_rel_pos = full_coord_size * 1;
    // 8
    const std::size_t full_coord_z_rel_pos = full_coord_size * 2;

    // 12
    const std::size_t compact_coord_x_rel_pos = full_coords_size +
                                                compact_coord_size * 0;
    // 13
    const std::size_t compact_coord_y_rel_pos = full_coords_size +
                                                compact_coord_size * 1;
    // 14
    const std::size_t compact_coord_z_rel_pos = full_coords_size +
                                                compact_coord_size * 2;

    // 15
    const std::size_t sort_info_rel_pos = full_coords_size +
                                          compact_coords_size;
  } // namespace vertex



  namespace normal{
    const int default_sort_info = 0;
    const unsigned char default_n_power = 0;

    // 1  char
    const std::size_t coord_size = sizeof(char);
    // 3
    const std::size_t coords_size = coord_size * volInt::axes_num;

    // 1 unsigned char
    const std::size_t n_power_size = sizeof(unsigned char);

    // 4  int
    const std::size_t sort_info_size = sizeof(int);

    // 5
    const std::size_t size_no_coords = n_power_size + sort_info_size;

    // 8
    const std::size_t size = coords_size + n_power_size + sort_info_size;

    // 0
    const std::size_t coord_x_rel_pos = vertex::full_coord_size * 0;
    // 1
    const std::size_t coord_y_rel_pos = vertex::full_coord_size * 1;
    // 2
    const std::size_t coord_z_rel_pos = vertex::full_coord_size * 2;

    // 3
    const std::size_t n_power_rel_pos = coords_size;

    // 4
    const std::size_t sort_info_rel_pos = coords_size + n_power_size;
  } // namespace normal



  namespace polygon{
    const unsigned int default_color_shift = 0;
    const int default_sort_info = 0;
    const unsigned char default_flat_norm_n_power = 0;


    // 4 int
    const std::size_t vertex_num_size = sizeof(int);
    // 4 int
    const std::size_t sort_info_size = sizeof(int);
    // 4 unsigned int
    const std::size_t color_id_size = sizeof(unsigned int);
    // 4 unsigned int
    const std::size_t color_shift_size = sizeof(unsigned int);
    // 1 char
    const std::size_t flat_normal_coord_size = sizeof(char);
    // 1 unsigned char
    const std::size_t flat_normal_n_power_size = sizeof(unsigned char);
    // 4
    const std::size_t flat_normal_size =
      flat_normal_coord_size * volInt::axes_num + flat_normal_n_power_size;
    // 1 char
    const std::size_t middle_point_coord_size = sizeof(char);
    // 3
    const std::size_t middle_point_size =
      middle_point_coord_size * volInt::axes_num;


    // 4 int
    const std::size_t vert_ind_size = sizeof(int);
    // 4 int
    const std::size_t norm_ind_size = sizeof(int);



    // 0
    const std::size_t vertex_num_rel_pos = 0;
    // 4
    const std::size_t sort_info_rel_pos = vertex_num_size;
    // 8
    const std::size_t color_id_rel_pos =
      sort_info_rel_pos + sort_info_size;
    // 12
    const std::size_t color_shift_rel_pos =
      color_id_rel_pos + color_id_size;
    // 16
    const std::size_t flat_normal_x_rel_pos =
      color_shift_rel_pos + color_shift_size;
    // 17
    const std::size_t flat_normal_y_rel_pos =
      flat_normal_x_rel_pos + flat_normal_coord_size;
    // 18
    const std::size_t flat_normal_z_rel_pos =
      flat_normal_y_rel_pos + flat_normal_coord_size;
    // 19
    const std::size_t flat_normal_n_power_rel_pos =
      flat_normal_z_rel_pos + flat_normal_coord_size;
    // 20
    const std::size_t middle_point_x_rel_pos =
      flat_normal_n_power_rel_pos + flat_normal_n_power_size;
    // 21
    const std::size_t middle_point_y_rel_pos =
      middle_point_x_rel_pos + middle_point_coord_size;
    // 22
    const std::size_t middle_point_z_rel_pos =
      middle_point_y_rel_pos + middle_point_coord_size;

    // 23
    const std::size_t vert_norm_ind_arr_rel_pos =
      middle_point_z_rel_pos + middle_point_coord_size;

    const std::size_t general_info_size = vert_norm_ind_arr_rel_pos;
    const std::size_t size_per_vertex = vert_ind_size + norm_ind_size;
  } // namespace polygon

  namespace polygon_sort_info{
    // 4 int
    const std::size_t poly_ind_size = sizeof(int);
    // 12
    const std::size_t size = poly_ind_size * 3;

    // 0
    const std::size_t poly_ind_rel_pos = 0;
  } // namespace polygon_sort_info



  namespace color{
    const int max_offset = 255;
    const int max_shift = 7;

    const int default_offset = 128;
    const int default_shift = 3;

    struct id{};
    struct name{};

    typedef boost::bimaps::bimap
    <
      boost::bimaps::unordered_set_of<
        boost::bimaps::tagged< unsigned int, id   > >,
      boost::bimaps::unordered_set_of< boost::bimaps::tagged<
        std::string , name > >,
      boost::bimaps::unconstrained_set_of_relation
    > ids_type;


    struct offset_pair
    {
      offset_pair()
      : offset(0), shift(0)
      {
      }

      offset_pair(int offset_arg, int shift_arg)
      : offset(offset_arg), shift(shift_arg)
      {
      }

      offset_pair(std::initializer_list<int> l)
      {
        offset = *l.begin();
        shift = *std::next(l.begin());
      }

      int offset;
      int shift;
    };

    typedef std::unordered_map<std::string, offset_pair> offset_map;


    const ids_type ids = boost::assign::list_of<ids_type::relation>
      (0 , "zero_reserved")
      (1 , "body")
      (2 , "window")
      (3 , "wheel")
      (4 , "defense")
      (5 , "weapon")
      (6 , "tube")
      (7 , "body_red")
      (8 , "body_blue")
      (9 , "body_yellow")
      (10, "body_gray")
      (11, "yellow_charged")
      (12, "material_0")
      (13, "material_1")
      (14, "material_2")
      (15, "material_3")
      (16, "material_4")
      (17, "material_5")
      (18, "material_6")
      (19, "material_7")
      (20, "black")
      (21, "body_green")
      (22, "skyfarmer_kernoboo")
      (23, "skyfarmer_pipetka")
      (24, "rotten_item")
      (25, "max_colors_ids") // end of color id list in vangers source
      // doesn't exist in vangers source
      (1000000, "virtual_object_ids_begin")
      (1000001, "center_of_mass")
      (1000002, "attachment_point")
      (1999999, "virtual_object_ids_end")
//    (1000002, "no_material")
      ;



    const std::vector<unsigned int> wheel_weapon_color_ids
    {
      3,       // wheel
      5,       // weapon
      1000002, // attachment_point
    };


    // c3d::color::string_to_id::
    namespace string_to_id
    {
      const unsigned int zero_reserved =            0;
      const unsigned int body =                     1;
      const unsigned int window =                   2;
      const unsigned int wheel =                    3;
      const unsigned int defense =                  4;
      const unsigned int weapon =                   5;
      const unsigned int tube =                     6;
      const unsigned int body_red =                 7;
      const unsigned int body_blue =                8;
      const unsigned int body_yellow =              9;
      const unsigned int body_gray =                10;
      const unsigned int yellow_charged =           11;
      const unsigned int material_0 =               12;
      const unsigned int material_1 =               13;
      const unsigned int material_2 =               14;
      const unsigned int material_3 =               15;
      const unsigned int material_4 =               16;
      const unsigned int material_5 =               17;
      const unsigned int material_6 =               18;
      const unsigned int material_7 =               19;
      const unsigned int black =                    20;
      const unsigned int body_green =               21;
      const unsigned int skyfarmer_kernoboo =       22;
      const unsigned int skyfarmer_pipetka =        23;
      const unsigned int rotten_item =              24;
      const unsigned int max_colors_ids =           25; // end of color id list
                                                        //   in vangers source
      // doesn't exist in vangers source
      const unsigned int virtual_object_ids_begin = 1000000;
      const unsigned int center_of_mass =           1000001;
      const unsigned int attachment_point =         1000002;
      const unsigned int virtual_object_ids_end =   1999999;
    }



    const std::vector<unsigned int> id_to_shift
    {
      0, // (0 , "zero_reserved")
      3, // (1 , "body")
      4, // (2 , "window")
      7, // (3 , "wheel")
      4, // (4 , "defense")
      3, // (5 , "weapon")
      7, // (6 , "tube")
      3, // (7 , "body_red")
      3, // (8 , "body_blue")
      3, // (9 , "body_yellow")
      4, // (10, "body_gray")
      4, // (11, "yellow_charged")
      2, // (12, "material_0")
      2, // (13, "material_1")
      4, // (14, "material_2")
      3, // (15, "material_3")
      3, // (16, "material_4")
      4, // (17, "material_5")
      4, // (18, "material_6")
      4, // (19, "material_7")
      4, // (20, "black")
      3, // (21, "body_green")
      4, // (22, "skyfarmer_kernoboo")
      4, // (23, "skyfarmer_pipetka")
      4, // (24, "rotten_item")
    };



    const offset_map offsets
    {
      {"zero_reserved",      {0  , 0}},
//    {"body",               {128, 3}},
      {"window",             {176, 4}},
//    {"wheel",              {224, 7}},
      {"defense",            {184, 4}},
//    {"weapon",             {224, 3}},
      {"tube",               {224, 7}},
      {"body_red",           {128, 3}},
      {"body_blue",          {144, 3}},
      {"body_yellow",        {160, 3}},
      {"body_gray",          {228, 4}},
      {"yellow_charged",     {112, 4}},
      {"material_0",         {0  , 2}},
      {"material_1",         {32 , 2}},
      {"material_2",         {64 , 4}},
      {"material_3",         {72 , 3}},
      {"material_4",         {88 , 3}},
      {"material_5",         {104, 4}},
      {"material_6",         {112, 4}},
      {"material_7",         {120, 4}},
      {"black",              {184, 4}},
      {"body_green",         {240, 3}},
      {"skyfarmer_kernoboo", {136, 4}},
      {"skyfarmer_pipetka",  {128, 4}},
      {"rotten_item",        {224, 4}},
      // doesn't exist in vangers source
//    {"center_of_mass",   {128, 4}},
//    {"attachment_point",   {240, 4}},
    };
    const offset_map default_body_offsets
    {
      {"body_offset_0_shift_0",   {0  , 0}},
      {"body_offset_34_shift_3",  {34 , 3}},
      {"body_offset_128_shift_3", {128, 3}},
      {"body_offset_144_shift_3", {144, 3}},
      {"body_offset_145_shift_3", {145, 3}},
      {"body_offset_160_shift_3", {160, 3}},
    };

    const offset_pair wheel_offset           {224, 7};
    const offset_pair weapon_offset          {224, 3};
    // doesn't exist in vangers source
    const offset_pair center_of_mass_offset  {128, 4};
    const offset_pair attachment_point_offset{240, 4};
  } // namespace color

} // namespace c3d



namespace m3d{

  namespace header{
    // 4 int
    const std::size_t xmax_size = sizeof(int);
    // 4 int
    const std::size_t ymax_size = sizeof(int);
    // 4 int
    const std::size_t zmax_size = sizeof(int);
    // 4 int
    const std::size_t rmax_size = sizeof(int);

    // 4 int
    const std::size_t n_wheels_size = sizeof(int);
    // 4 int
    const std::size_t n_debris_size = sizeof(int);

    // 4 int
    const std::size_t body_color_offset_size = sizeof(int);
    // 4 int
    const std::size_t body_color_shift_size = sizeof(int);

    // 32
    const std::size_t size =
      xmax_size + ymax_size + zmax_size + rmax_size +
      n_wheels_size + n_debris_size + body_color_offset_size +
      body_color_shift_size;


    // 0 int
    const std::size_t xmax_pos = 0;
    // 4 int
    const std::size_t ymax_pos = xmax_pos + sizeof(int);
    // 8 int
    const std::size_t zmax_pos = ymax_pos + sizeof(int);
    // 12 int
    const std::size_t rmax_pos = zmax_pos + sizeof(int);

    // 16 int
    const std::size_t n_wheels_pos = rmax_pos + sizeof(int);
    // 20 int
    const std::size_t n_debris_pos = n_wheels_pos + sizeof(int);

    // 24 int
    const std::size_t body_color_offset_pos = n_debris_pos + sizeof(int);
    // 28 int
    const std::size_t body_color_shift_pos =
      body_color_offset_pos + sizeof(int);
  } // namespace header

  namespace wheel{
    const int default_bound_index = 0;

    // 4 int
    const std::size_t steer_size = sizeof(int);
    // 24 double vector
    const std::size_t r_size = sizeof(double) * volInt::axes_num;
    // 4 int
    const std::size_t width_size = sizeof(int);
    // 4 int
    const std::size_t radius_size = sizeof(int);
    // 4 int
    const std::size_t bound_index_size = sizeof(int);

    // 40
    const std::size_t size =
      steer_size + r_size + width_size + radius_size + bound_index_size;


    // 0 int
    const std::size_t steer_pos = 0;
    // 4 double vector
    const std::size_t r_pos = steer_pos + sizeof(int);
    // 28 int
    const std::size_t width_pos = r_pos + sizeof(double) * volInt::axes_num;
    // 32 int
    const std::size_t radius_pos = width_pos + sizeof(int);
    // 36 int
    const std::size_t bound_index_pos = radius_pos + sizeof(int);
  } // namespace wheel

  namespace weapon_slot{
    const std::size_t max_slots = 3;

    // 4 int
    const std::size_t slots_existence_size = sizeof(int);

    // 12 int vector
    const std::size_t R_slot_size = sizeof(int) * volInt::axes_num;
    // 4 int
    const std::size_t location_angle_of_slot_size = sizeof(int);

    // 16
    const std::size_t slot_data_size =
      R_slot_size + location_angle_of_slot_size;


    // 0 int
    const std::size_t slots_existence_pos = 0;

    // 0 int vector
    const std::size_t R_slot_rel_pos = 0;
    // 12 int
    const std::size_t location_angle_of_slot_rel_pos =
      R_slot_rel_pos + sizeof(int) * volInt::axes_num;
  } // namespace weapon_slot

} // namespace m3d

namespace a3d{

  namespace header{
    // 4 int
    const std::size_t n_models_size = sizeof(int);
    // 4 int
    const std::size_t xmax_size = sizeof(int);
    // 4 int
    const std::size_t ymax_size = sizeof(int);
    // 4 int
    const std::size_t zmax_size = sizeof(int);
    // 4 int
    const std::size_t rmax_size = sizeof(int);

    // 4 int
    const std::size_t body_color_offset_size = sizeof(int);
    // 4 int
    const std::size_t body_color_shift_size = sizeof(int);

    // 32
    const std::size_t size =
      n_models_size + xmax_size + ymax_size + zmax_size + rmax_size +
      body_color_offset_size + body_color_shift_size;


    // 0 int
    const std::size_t n_models_pos = 0;
    // 4 int
    const std::size_t xmax_pos = n_models_pos + sizeof(int);
    // 8 int
    const std::size_t ymax_pos = xmax_pos + sizeof(int);
    // 12 int
    const std::size_t zmax_pos = ymax_pos + sizeof(int);
    // 16 int
    const std::size_t rmax_pos = zmax_pos + sizeof(int);

    // 20 int
    const std::size_t body_color_offset_pos = rmax_pos + sizeof(int);
    // 24 int
    const std::size_t body_color_shift_pos =
      body_color_offset_pos + sizeof(int);
  } // namespace header

} // namespace a3d

} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_VANGERS_MODELS_CONSTANTS_H
