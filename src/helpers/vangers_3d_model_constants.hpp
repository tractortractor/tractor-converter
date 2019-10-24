#ifndef TRACTOR_CONVERTER_VANGERS_MODELS_CONSTANTS_H
#define TRACTOR_CONVERTER_VANGERS_MODELS_CONSTANTS_H

#include "defines.hpp"

#include "volInt.hpp"

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
    main_of_mechos,
    bound,
  };

  const int version_req = 8;

  const std::vector<int> default_phi_psi_tetta = std::vector<int>(3, 83);

  const std::size_t regular_model_vertices_per_polygon = 3;
  const std::size_t bound_model_vertices_per_polygon = 4;



  namespace header{
    // 0   int
    const std::size_t version_pos = 0;
    // 4   int
    const std::size_t num_vert_pos = version_pos + sizeof(int);
    // 8   int
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


    // 72  double
    const std::size_t volume_pos = tetta_pos + sizeof(int);
    // 80  double vector
    const std::size_t rcm_pos = volume_pos + sizeof(double);
    // 104 double matrix
    const std::size_t J_pos = rcm_pos + sizeof(double) * volInt::axes_num;



    // 176
    const std::size_t size =
      J_pos + sizeof(double) * volInt::axes_num * volInt::axes_num;
  } // namespace header

  const std::size_t data_pos = header::size;



  namespace vertex{
    const int default_sort_info = 0;

    // 4   float
    const std::size_t full_coord_size = sizeof(float);
    // 12
    const std::size_t full_coords_size =
      full_coord_size * volInt::axes_num;

    // 1   char
    const std::size_t compact_coord_size = sizeof(char);
    // 3
    const std::size_t compact_coords_size =
      compact_coord_size * volInt::axes_num;

    // 4   int
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

    // 1   char
    const std::size_t coord_size = sizeof(char);
    // 3
    const std::size_t coords_size = coord_size * volInt::axes_num;

    // 1   unsigned char
    const std::size_t n_power_size = sizeof(unsigned char);

    // 4   int
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


    // 4   int
    const std::size_t vertex_num_size = sizeof(int);
    // 4   int
    const std::size_t sort_info_size = sizeof(int);
    // 4   unsigned int
    const std::size_t color_id_size = sizeof(unsigned int);
    // 4   unsigned int
    const std::size_t color_shift_size = sizeof(unsigned int);
    // 1   char
    const std::size_t flat_normal_coord_size = sizeof(char);
    // 1   unsigned char
    const std::size_t flat_normal_n_power_size = sizeof(unsigned char);
    // 4
    const std::size_t flat_normal_size =
      flat_normal_coord_size * volInt::axes_num + flat_normal_n_power_size;
    // 1   char
    const std::size_t middle_point_coord_size = sizeof(char);
    // 3
    const std::size_t middle_point_size =
      middle_point_coord_size * volInt::axes_num;


    // 4   int
    const std::size_t vert_ind_size = sizeof(int);
    // 4   int
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
    // 4   int
    const std::size_t poly_ind_size = sizeof(int);
    // 12
    const std::size_t size = poly_ind_size * 3;

    // 0
    const std::size_t poly_ind_rel_pos = 0;
  } // namespace polygon_sort_info



  namespace color{
    namespace string{
      const std::string zero_reserved =            "zero_reserved";
      const std::string body =                     "body";
      const std::string window =                   "window";
      const std::string wheel =                    "wheel";
      const std::string defense =                  "defense";
      const std::string weapon =                   "weapon";
      const std::string tube =                     "tube";
      const std::string body_red =                 "body_red";
      const std::string body_blue =                "body_blue";
      const std::string body_yellow =              "body_yellow";
      const std::string body_gray =                "body_gray";
      const std::string yellow_charged =           "yellow_charged";
      const std::string material_0 =               "material_0";
      const std::string material_1 =               "material_1";
      const std::string material_2 =               "material_2";
      const std::string material_3 =               "material_3";
      const std::string material_4 =               "material_4";
      const std::string material_5 =               "material_5";
      const std::string material_6 =               "material_6";
      const std::string material_7 =               "material_7";
      const std::string black =                    "black";
      const std::string body_green =               "body_green";
      const std::string skyfarmer_kernoboo =       "skyfarmer_kernoboo";
      const std::string skyfarmer_pipetka =        "skyfarmer_pipetka";
      const std::string rotten_item =              "rotten_item";
      // End of color id list in Vangers source.
      const std::string max_colors_ids =           "max_colors_ids";

      // Those don't exist in Vangers source.
      const std::string virtual_object_ids_begin = "virtual_object_ids_begin";
      const std::string center_of_mass =           "center_of_mass";
      const std::string attachment_point =         "attachment_point";
      const std::string invalid_color_id =         "invalid_color_id";
      const std::string virtual_object_ids_end =   "virtual_object_ids_end";
    } // namespace string

    namespace string_to_id{
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
      // End of color id list in Vangers source.
      const unsigned int max_colors_ids =           25;

      // Those don't exist in Vangers source.
      const unsigned int virtual_object_ids_begin = 1000000;
      const unsigned int center_of_mass =           1000001;
      const unsigned int attachment_point =         1000002;
      const unsigned int invalid_color_id =         1000003;
      const unsigned int virtual_object_ids_end =   1999999;
    } // namespace string_to_id


    namespace body{
      namespace string{
        const std::string separator = "_";
        const std::string offset =    "offset";
        const std::string shift =     "shift";
        const std::string el_1 =
          c3d::color::string::body + separator + offset + separator;
        const std::string el_2 = separator + shift + separator;
      } // namespace string
      namespace max{
        const int offset = 255;
        const int shift = 7;
      } // namespace max
      namespace min{
        const int offset = 0;
        const int shift = 0;
      } // namespace min
      namespace default_val{
        const int offset = 128;
        const int shift = 3;
      } // namespace default_val
    } // namespace body

    struct id{};
    struct name{};

    typedef boost::bimaps::bimap
    <
      boost::bimaps::unordered_set_of<
        boost::bimaps::tagged<unsigned int, id>>,
      boost::bimaps::unordered_set_of< boost::bimaps::tagged<
        std::string, name>>,
      boost::bimaps::unconstrained_set_of_relation
    > ids_type;



    const ids_type ids = boost::assign::list_of<ids_type::relation>
      (string_to_id::zero_reserved, string::zero_reserved)
      (string_to_id::body, string::body)
      (string_to_id::window, string::window)
      (string_to_id::wheel, string::wheel)
      (string_to_id::defense, string::defense)
      (string_to_id::weapon, string::weapon)
      (string_to_id::tube, string::tube)
      (string_to_id::body_red, string::body_red)
      (string_to_id::body_blue, string::body_blue)
      (string_to_id::body_yellow, string::body_yellow)
      (string_to_id::body_gray, string::body_gray)
      (string_to_id::yellow_charged, string::yellow_charged)
      (string_to_id::material_0, string::material_0)
      (string_to_id::material_1, string::material_1)
      (string_to_id::material_2, string::material_2)
      (string_to_id::material_3, string::material_3)
      (string_to_id::material_4, string::material_4)
      (string_to_id::material_5, string::material_5)
      (string_to_id::material_6, string::material_6)
      (string_to_id::material_7, string::material_7)
      (string_to_id::black, string::black)
      (string_to_id::body_green, string::body_green)
      (string_to_id::skyfarmer_kernoboo, string::skyfarmer_kernoboo)
      (string_to_id::skyfarmer_pipetka, string::skyfarmer_pipetka)
      (string_to_id::rotten_item, string::rotten_item)
      // End of color id list in Vangers source.
      (string_to_id::max_colors_ids, string::max_colors_ids)
      // Those don't exist in Vangers source.
      (string_to_id::virtual_object_ids_begin,
         string::virtual_object_ids_begin)
      (string_to_id::center_of_mass, string::center_of_mass)
      (string_to_id::attachment_point, string::attachment_point)
      (string_to_id::invalid_color_id, string::invalid_color_id)
      (string_to_id::virtual_object_ids_end, string::virtual_object_ids_end)
      ;
  } // namespace color

} // namespace c3d



namespace m3d{

  namespace header{
    // 4   int
    const std::size_t xmax_size = sizeof(int);
    // 4   int
    const std::size_t ymax_size = sizeof(int);
    // 4   int
    const std::size_t zmax_size = sizeof(int);
    // 4   int
    const std::size_t rmax_size = sizeof(int);

    // 4   int
    const std::size_t n_wheels_size = sizeof(int);
    // 4   int
    const std::size_t n_debris_size = sizeof(int);

    // 4   int
    const std::size_t body_color_offset_size = sizeof(int);
    // 4   int
    const std::size_t body_color_shift_size = sizeof(int);

    // 32
    const std::size_t size =
      xmax_size + ymax_size + zmax_size + rmax_size +
      n_wheels_size + n_debris_size + body_color_offset_size +
      body_color_shift_size;


    // 0   int
    const std::size_t xmax_pos = 0;
    // 4   int
    const std::size_t ymax_pos = xmax_pos + sizeof(int);
    // 8   int
    const std::size_t zmax_pos = ymax_pos + sizeof(int);
    // 12  int
    const std::size_t rmax_pos = zmax_pos + sizeof(int);

    // 16  int
    const std::size_t n_wheels_pos = rmax_pos + sizeof(int);
    // 20  int
    const std::size_t n_debris_pos = n_wheels_pos + sizeof(int);

    // 24  int
    const std::size_t body_color_offset_pos = n_debris_pos + sizeof(int);
    // 28  int
    const std::size_t body_color_shift_pos =
      body_color_offset_pos + sizeof(int);
  } // namespace header

  namespace wheel{
    const int default_bound_index = 0;

    // 4   int
    const std::size_t steer_size = sizeof(int);
    // 24  double vector
    const std::size_t r_size = sizeof(double) * volInt::axes_num;
    // 4   int
    const std::size_t width_size = sizeof(int);
    // 4   int
    const std::size_t radius_size = sizeof(int);
    // 4   int
    const std::size_t bound_index_size = sizeof(int);

    // 40
    const std::size_t size =
      steer_size + r_size + width_size + radius_size + bound_index_size;


    // 0   int
    const std::size_t steer_pos = 0;
    // 4   double vector
    const std::size_t r_pos = steer_pos + sizeof(int);
    // 28  int
    const std::size_t width_pos = r_pos + sizeof(double) * volInt::axes_num;
    // 32  int
    const std::size_t radius_pos = width_pos + sizeof(int);
    // 36  int
    const std::size_t bound_index_pos = radius_pos + sizeof(int);
  } // namespace wheel

  namespace weapon_slot{
    const std::size_t max_slots = 3;

    // 4   int
    const std::size_t slots_existence_size = sizeof(int);

    // 12  int vector
    const std::size_t R_slot_size = sizeof(int) * volInt::axes_num;
    // 4   int
    const std::size_t location_angle_of_slot_size = sizeof(int);

    // 16
    const std::size_t slot_data_size =
      R_slot_size + location_angle_of_slot_size;


    // 0   int
    const std::size_t slots_existence_pos = 0;

    // 0   int vector
    const std::size_t R_slot_rel_pos = 0;
    // 12  int
    const std::size_t location_angle_of_slot_rel_pos =
      R_slot_rel_pos + sizeof(int) * volInt::axes_num;
  } // namespace weapon_slot

} // namespace m3d

namespace a3d{

  namespace header{
    // 4   int
    const std::size_t n_models_size = sizeof(int);
    // 4   int
    const std::size_t xmax_size = sizeof(int);
    // 4   int
    const std::size_t ymax_size = sizeof(int);
    // 4   int
    const std::size_t zmax_size = sizeof(int);
    // 4   int
    const std::size_t rmax_size = sizeof(int);

    // 4   int
    const std::size_t body_color_offset_size = sizeof(int);
    // 4   int
    const std::size_t body_color_shift_size = sizeof(int);

    // 32
    const std::size_t size =
      n_models_size + xmax_size + ymax_size + zmax_size + rmax_size +
      body_color_offset_size + body_color_shift_size;


    // 0   int
    const std::size_t n_models_pos = 0;
    // 4   int
    const std::size_t xmax_pos = n_models_pos + sizeof(int);
    // 8   int
    const std::size_t ymax_pos = xmax_pos + sizeof(int);
    // 12  int
    const std::size_t zmax_pos = ymax_pos + sizeof(int);
    // 16  int
    const std::size_t rmax_pos = zmax_pos + sizeof(int);

    // 20  int
    const std::size_t body_color_offset_pos = rmax_pos + sizeof(int);
    // 24  int
    const std::size_t body_color_shift_pos =
      body_color_offset_pos + sizeof(int);
  } // namespace header

} // namespace a3d



namespace mat_tables{

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



  namespace append_order_el{
    const std::string regular = "regular";
    const std::string special = "special";
    const std::string default_body = "default_body";
    const std::string additional_body = "additional_body";
  } // namespace append_order_el

  namespace html{
    const std::vector<std::string> append_order
      {
        append_order_el::regular,
        append_order_el::default_body,
        append_order_el::additional_body,
      };
  } // namespace html



  namespace mtl{
    const std::vector<std::string> append_order
      {
        append_order_el::regular,
        append_order_el::special,
        append_order_el::default_body,
        append_order_el::additional_body,
      };
  } // namespace mtl



  const offset_map regular_offsets
  {
    {c3d::color::string::zero_reserved,      {0  , 0}},
    {c3d::color::string::body,               {128, 3}},
    {c3d::color::string::window,             {176, 4}},
    {c3d::color::string::wheel,              {224, 7}},
    {c3d::color::string::defense,            {184, 4}},
    {c3d::color::string::weapon,             {224, 3}},
    {c3d::color::string::tube,               {224, 7}},
    {c3d::color::string::body_red,           {128, 3}},
    {c3d::color::string::body_blue,          {144, 3}},
    {c3d::color::string::body_yellow,        {160, 3}},
    {c3d::color::string::body_gray,          {228, 4}},
    {c3d::color::string::yellow_charged,     {112, 4}},
    {c3d::color::string::material_0,         {0  , 2}},
    {c3d::color::string::material_1,         {32 , 2}},
    {c3d::color::string::material_2,         {64 , 4}},
    {c3d::color::string::material_3,         {72 , 3}},
    {c3d::color::string::material_4,         {88 , 3}},
    {c3d::color::string::material_5,         {104, 4}},
    {c3d::color::string::material_6,         {112, 4}},
    {c3d::color::string::material_7,         {120, 4}},
    {c3d::color::string::black,              {184, 4}},
    {c3d::color::string::body_green,         {240, 3}},
    {c3d::color::string::skyfarmer_kernoboo, {136, 4}},
    {c3d::color::string::skyfarmer_pipetka,  {128, 4}},
    {c3d::color::string::rotten_item,        {224, 4}},
  };

  // Those don't exist in Vangers source.
  const offset_map special_offsets
  {
    {c3d::color::string::center_of_mass,     {128, 4}},
    {c3d::color::string::attachment_point,   {240, 4}},
  };

  const offset_map default_body_offsets
  {
    {c3d::color::body::string::el_1 + "0" +
       c3d::color::body::string::el_2 + "0", {0  , 0}},
    {c3d::color::body::string::el_1 + "34" +
       c3d::color::body::string::el_2 + "3", {34 , 3}},
    {c3d::color::body::string::el_1 + "112" +
       c3d::color::body::string::el_2 + "4", {112, 4}},
    {c3d::color::body::string::el_1 + "128" +
       c3d::color::body::string::el_2 + "3", {128, 3}},
    {c3d::color::body::string::el_1 + "128" +
       c3d::color::body::string::el_2 + "4", {128, 4}},
    {c3d::color::body::string::el_1 + "134" +
       c3d::color::body::string::el_2 + "4", {134, 4}},
    {c3d::color::body::string::el_1 + "136" +
       c3d::color::body::string::el_2 + "4", {136, 4}},
    {c3d::color::body::string::el_1 + "140" +
       c3d::color::body::string::el_2 + "5", {140, 5}},
    {c3d::color::body::string::el_1 + "144" +
       c3d::color::body::string::el_2 + "3", {144, 3}},
    {c3d::color::body::string::el_1 + "145" +
       c3d::color::body::string::el_2 + "3", {145, 3}},
    {c3d::color::body::string::el_1 + "152" +
       c3d::color::body::string::el_2 + "4", {152, 4}},
    {c3d::color::body::string::el_1 + "160" +
       c3d::color::body::string::el_2 + "3", {160, 3}},
    {c3d::color::body::string::el_1 + "160" +
       c3d::color::body::string::el_2 + "4", {160, 4}},
    {c3d::color::body::string::el_1 + "228" +
       c3d::color::body::string::el_2 + "4", {228, 4}},
    {c3d::color::body::string::el_1 + "240" +
       c3d::color::body::string::el_2 + "3", {240, 3}},
    {c3d::color::body::string::el_1 + "245" +
       c3d::color::body::string::el_2 + "4", {245, 4}},
  };

} // namespace mat_tables



} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_VANGERS_MODELS_CONSTANTS_H
