#ifndef VOLINT_H
#define VOLINT_H

        /*******************************************************
        *                                                      *
        *  volInt.c                                            *
        *                                                      *
        *  This code computes volume integrals needed for      *
        *  determining mass properties of polyhedral bodies.   *
        *                                                      *
        *  For more information, see the accompanying README   *
        *  file, and the paper                                 *
        *                                                      *
        *  Brian Mirtich, "Fast and Accurate Computation of    *
        *  Polyhedral Mass Properties," journal of graphics    *
        *  tools, volume 1, number 1, 1996.                    *
        *                                                      *
        *  This source code is public domain, and may be used  *
        *  in any way, shape or form, free of charge.          *
        *                                                      *
        *  Copyright 1995 by Brian Mirtich                     *
        *                                                      *
        *  mirtich@cs.berkeley.edu                             *
        *  http://www.cs.berkeley.edu/~mirtich                 *
        *                                                      *
        *******************************************************/

/*
        Revision history

        26 Jan 1996     Program creation.

         3 Aug 1996     Corrected bug arising when polyhedron density
                        is not 1.0.  Changes confined to function main().
                        Thanks to Zoran Popovic for catching this one.

        27 May 1997     Corrected sign error in translation of inertia
                        product terms to center of mass frame.  Changes 
                        confined to function main().  Thanks to 
                        Chris Hecker.
*/



// Modified for tractor_converter



#include <boost/container_hash/hash.hpp>

#include <stdexcept>

#include <algorithm>
#include <cmath>
#include <utility>
#include <limits>
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <functional>
#include <numeric>
#include <iterator>



#include <stdio.h>
#include <string.h>
#include <math.h>



namespace volInt{



namespace exception{
  struct negative_volume : public virtual std::runtime_error
  {
    using std::runtime_error::runtime_error;
  };
  struct zero_volume : public virtual std::runtime_error
  {
    using std::runtime_error::runtime_error;
  };
} // namespace exception



enum class rotation_axis : std::size_t {x = 0, y = 1, z = 2};



double degrees_to_radians(double degrees);
double radians_to_degrees(double radians);

double sicher_angle_to_radians(int sicher_angle);
int radians_to_sicher_angle(double radians);

void rotate_point_by_axis(
  std::vector<double> &point,
  double angle_sin,
  double angle_cos,
  rotation_axis axis);
void rotate_point_by_axis(
  std::vector<double> &point,
  double angle,
  rotation_axis axis);



std::vector<double> vector_scale(double norm, const std::vector<double> &vec);
void vector_scale_self(double norm, std::vector<double> &vec);

void vector_make_zero(std::vector<double> &vec);

std::vector<double> vector_invert(const std::vector<double> &vec);
void vector_invert_self(std::vector<double> &vec);

std::vector<double> vector_plus(const std::vector<double> &first,
                                const std::vector<double> &second);
void vector_plus_self(std::vector<double> &first,
                      const std::vector<double> &second);
std::vector<double> vector_minus(const std::vector<double> &first,
                                 const std::vector<double> &second);
void vector_minus_self(std::vector<double> &first,
                       const std::vector<double> &second);

std::vector<double> vector_multiply(const std::vector<double> &first,
                                    const std::vector<double> &second);
void vector_multiply_self(std::vector<double> &first,
                          const std::vector<double> &second);
std::vector<double> vector_multiply(const std::vector<double> &vec,
                                    double num);
void vector_multiply_self(std::vector<double> &vec,
                          double num);

std::vector<double> vector_divide(const std::vector<double> &first,
                                  const std::vector<double> &second);
void vector_divide_self(std::vector<double> &first,
                        const std::vector<double> &second);
std::vector<double> vector_divide(const std::vector<double> &vec,
                                  double num);
void vector_divide_self(std::vector<double> &vec,
                        double num);

double vector_length(const std::vector<double> &vec);

double vector_length_between(
  const std::vector<double> &first,
  const std::vector<double> &second);

std::vector<double> vector_middle(
  const std::vector<double> &first,
  const std::vector<double> &second);

bool vector_equal(
  const std::vector<double> &first,
  const std::vector<double> &second);

double vector_dot_product(
  const std::vector<double> &first,
  const std::vector<double> &second);

std::vector<double> vector_cross_product(
  const std::vector<double> &first,
  const std::vector<double> &second);

double vector_angle(
  const std::vector<double> &first,
  const std::vector<double> &second);

std::vector<double> vector_2d_minus(const std::vector<double> &first,
                                    const std::vector<double> &second);
void vector_2d_minus_self(std::vector<double> &first,
                          const std::vector<double> &second);

std::vector<double> vector_2d_divide(const std::vector<double> &vec,
                                     double num);

double vector_2d_length(const std::vector<double> &vec);

void matrix_multiply_self(
  std::vector<std::vector<double>> &mat,
  double num);





template<typename T>
std::vector<std::vector<T>> get_groups_of_connected_items(
  std::vector<T> orig_vec,
  std::function<bool(T first, T second)> check_connected_func)
{
  std::vector<std::vector<T>> groups;
  std::size_t orig_vec_size = orig_vec.size();
  groups.reserve(orig_vec_size);

  std::unordered_set<T> items_to_check(orig_vec.begin(), orig_vec.end());

  for(std::size_t cur_group_id = 0; !items_to_check.empty(); ++cur_group_id)
  {
    groups.push_back(std::vector<T>());
    groups[cur_group_id].reserve(orig_vec_size);

    T cur_item = *items_to_check.begin();
    items_to_check.erase(items_to_check.begin());

    std::unordered_set<T> connected_items_to_check({cur_item});
    connected_items_to_check.reserve(orig_vec_size);

    // Iterating over first element in group and all connected elements.
    while(!connected_items_to_check.empty())
    {
      T cur_item = *connected_items_to_check.begin();
      connected_items_to_check.erase(connected_items_to_check.begin());
      groups[cur_group_id].push_back(cur_item);

      std::unordered_set<T> new_connected_items;
      new_connected_items.reserve(orig_vec_size);

      for(auto cur_item_to_cmp : items_to_check)
      {
        if(check_connected_func(cur_item, cur_item_to_cmp))
        {
          new_connected_items.insert(cur_item_to_cmp);
        }
      }

      // Erasing all newly found connected items from items to check.
      for(auto new_connected_item : new_connected_items)
      {
        items_to_check.erase(items_to_check.find(new_connected_item));
      }

      connected_items_to_check.insert(new_connected_items.begin(),
                                      new_connected_items.end());
    }
  }

  groups.shrink_to_fit();

  return groups;
}



namespace calc_norms{
  std::size_t normal_to_key(const std::vector<double> &norm);
  std::vector<double> key_to_normal(std::size_t key);
} // namespace calc_norms



/*
   ============================================================================
   macros
   ============================================================================
*/

#define VOLINT_SQR(x) ((x)*(x))
#define VOLINT_CUBE(x) ((x)*(x)*(x))

/*
   ============================================================================
   constants
   ============================================================================
*/

//#define MAX_VERTS 100     /* maximum number of polyhedral vertices */
//#define MAX_FACES 100     /* maximum number of polyhedral faces */
//#define MAX_POLYGON_SZ 10 /* maximum number of verts per polygonal face */

#define VOLINT_X 0
#define VOLINT_Y 1
#define VOLINT_Z 2



const double vector_scale_val = 1.0;



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



const int min_float_precision = 3;
const double distinct_distance =
  1.0/std::pow(10, min_float_precision);
//const double distinct_distance = 1.0e-3;
const double sqr_distinct_distance =
  VOLINT_SQR(distinct_distance);
const double density = 1.0;

const std::size_t axes_num = 3;
const std::size_t axes_2d_num = 2;

const std::vector<std::vector<std::size_t>> axes_by_plane =
  {
    {1, 2}, // x axis
    {0, 2}, // y axis
    {0, 1}, // z axis
  };

const std::vector<std::vector<std::size_t>> axes_by_plane_continuous =
  {
    {1, 2}, // x axis
    {2, 0}, // y axis
    {0, 1}, // z axis
  };

namespace color_ids {
  const unsigned int zero_reserved =  0;
  const unsigned int body =           1;
  const unsigned int max_colors_ids = 25;
} // namespace color_ids

namespace calc_norms{
  const std::size_t expected_connected_polygons_per_vertex = 10;
  const std::size_t expected_connected_polygons_per_polygon = 10;

  const double to_integer_multiply = std::pow(10, min_float_precision);
  const std::size_t upper_bound =
    std::round(2 * vector_scale_val * to_integer_multiply);

  const std::size_t upper_bound_shift =
    static_cast<std::size_t>(std::log2(upper_bound)) + 1;

  const std::vector<std::size_t> to_key_shift =
    {
      0,
      upper_bound_shift,
      2 * upper_bound_shift,
    };

  const std::size_t k_to_n_mask = (1 << upper_bound_shift) - 1;
} // namespace calc_norms

namespace generate_bound{
  typedef std::deque<std::size_t> layer_vert_inds;
  typedef std::map<std::size_t, layer_vert_inds> layers_inds_of_axis;
  typedef std::vector<layers_inds_of_axis> layers_inds_by_axis;

  const std::size_t expected_inter_verts_per_edge = 10;

  const std::size_t plane_extrs_num = 4;
  // Assumes that get_planes_4_extreme_points() generates points in this order.
  //   y
  // 3   2
  //       x
  // 0   1
  const std::vector<std::vector<std::size_t>> extr_lines =
    {
      {0, 1},
      {1, 2},
      {2, 3},
      {3, 0},
      {0, 1, 2, 3},
    };
  const std::size_t plane_middle_extr_num = extr_lines.size();


  const std::size_t end_z_layers_num_mechos = 2;
  const std::size_t end_z_layers_num_other = 3;

  enum class model_type{mechos, other};

  namespace model{

    const std::size_t verts_per_poly = 4;

    const std::vector<std::size_t> zero_reserved_face_inds = {4, 5, 6, 7};

    const std::size_t z_layers_num = 3;
    const std::size_t num_verts_per_z_layer = 9;
    const std::size_t num_verts = z_layers_num * num_verts_per_z_layer;
    // 6 sides * 4 polygons per side.
    const std::size_t num_faces = 6 * 4;

    //    y
    // e3 m2 e2
    // m3 m4 m1 x
    // e0 m0 e1
    // Converting to:
    //   y
    // 0 1 2
    // 3 4 5 x
    // 6 7 8
    const std::vector<std::size_t> extr_to_end =   {6, 8, 2, 0};
    const std::vector<std::size_t> middle_to_end = {7, 5, 1, 3, 4};


    const std::vector<std::vector<int>> face_ind_to_vert_inds =
      {
        // Getting top and low sides.
        // top
        // <-
        //  /\
        // 0 1 2
        // 3 4 5
        // 6 7 8
        {0, 3, 4, 1},
        {1, 4, 5, 2},
        {3, 6, 7, 4},
        {4, 7, 8, 5},
        // low
        // ->
        //  \/
        // 18 19 20
        // 21 22 23
        // 24 25 26
        {18, 19, 22, 21},
        {19, 20, 23, 22},
        {21, 22, 25, 24},
        {22, 23, 26, 25},


        // Getting front and back sides.
        // back
        // <-
        //  /\
        // 6  7  8
        // 15 16 17
        // 24 25 26
        {6,  15, 16, 7},
        {7,  16, 17, 8},
        {15, 24, 25, 16},
        {16, 25, 26, 17},
        // front
        // ->
        //  \/
        // 0  1  2
        // 9  10 11
        // 18 19 20
        {0,  1,  10, 9},
        {1,  2,  11, 10},
        {9,  10, 19, 18},
        {10, 11, 20, 19},



        // Getting left and right sides.
        // left
        // <-
        //  /\
        // 0  3  6
        // 9  12 15
        // 18 21 24
        {0,  9,  12, 3},
        {3,  12, 15, 6},
        {9,  18, 21, 12},
        {12, 21, 24, 15},
        // right
        // ->
        //  \/
        // 2  5  8
        // 11 14 17
        // 20 23 26
        {2,  5,  14, 11},
        {5,  8,  17, 14},
        {11, 14, 23, 20},
        {14, 17, 26, 23},
      };

    // low
    //        y
    //     18 19 20
    // -x  21 22 23  x
    //     24 25 26
    //       -y
    const std::vector<std::vector<std::size_t>> min_verts_to_adjust_by_wheel =
      {
        {18, 21, 24}, // -x
        {24, 25, 26}, // -y
      };
    const std::vector<std::vector<std::size_t>> max_verts_to_adjust_by_wheel =
      {
        {20, 23, 26}, // x
        {18, 19, 20}, // y
      };
    const std::size_t min_layer_vert_to_center = 22;
    const std::vector<std::size_t> min_layer_extremes = {18, 20, 24, 26};
  } // namespace model
} // namespace generate_bound

/*
   ============================================================================
   data structures
   ============================================================================
*/

struct model_extreme_points {

  model_extreme_points();
  model_extreme_points(const std::vector<double> &max,
                       const std::vector<double> &min);
  model_extreme_points(std::vector<double> &&max, std::vector<double> &&min);

  std::vector<double> &max();
  const std::vector<double> &max() const;
  std::vector<double> &min();
  const std::vector<double> &min() const;

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

  void get_most_extreme_cmp_cur(const model_extreme_points &other);
  void get_most_extreme_cmp_cur(const std::vector<double> &point);
  void get_most_extreme(const std::vector<std::vector<double>> &points);
  void get_most_extreme(const std::vector<const std::vector<double>*> &points);

  std::vector<double> get_center();

  std::pair<std::vector<double>, std::vector<double>> extreme_points_pair;

};

struct model_offset {

  model_offset();
  model_offset(const std::vector<double> &offset_point_arg);
  model_offset(std::vector<double> &&offset_point_arg);

  double x_off() const;
  double y_off() const;
  double z_off() const;

  void set_x_off(double new_x_off);
  void set_y_off(double new_y_off);
  void set_z_off(double new_z_off);

  std::vector<double> offset_point;

};

typedef struct face {

//  face(const face &other);
  face(int numVerts_arg);

  int numVerts;
  unsigned int color_id;
  // In case color_id is "wheel" or "weapon"
  // this value is used to keep id of wheel or weapon.
  // During conversion to *.obj file it will be easier to
  // generate material name for polygon.
  int wheel_weapon_id;
//  double norm[3];
  std::vector<double> norm;
  double w;
//  int verts[MAX_POLYGON_SZ];
  std::vector<int> verts;
  std::vector<int> vertNorms;
//  struct polyhedron *poly;
} FACE;

typedef struct polyhedron {

  polyhedron();
//  polyhedron(const polyhedron &other);
//  polyhedron(polyhedron &&other);
  polyhedron(
    int numVerts_arg,
    int numVertNorms_arg,
    int numFaces_arg,
    int verts_per_poly_arg);

  void invertVertNorms();
  void reverse_polygons_orientation();

  std::vector<double> face_calc_normal(std::size_t face_ind);
  double face_calc_offset_w(std::size_t face_ind);
  void faces_calc_params(); // Must be called again if model was moved.
  void faces_calc_params_inv_neg_vol();

  double get_vertex_angle(std::size_t face_ind, std::size_t vert_ind);

  void recalc_vertNorms(double max_smooth_angle);

  double check_volume();

  void get_extreme_points();
  model_extreme_points get_extreme_points(
    const std::vector<const face*> &polygons) const;


  std::vector<double> get_model_center();
  std::vector<double> get_model_center(
    const std::vector<const std::vector<double>*> &vertices) const;
  std::vector<double> get_model_center(
    const std::vector<const face*> &polygons) const;

  std::vector<const face*>
    get_polygons_by_color(unsigned int color_id) const;
  std::vector<const face*> get_polygons_by_ids(
    unsigned int color_id,
    int wheel_weapon_id) const;

  std::vector<const std::vector<double>*> get_vertices_by_polygons(
    const std::vector<const face*> &model_polygons) const;

  std::vector<const std::vector<double>*> get_vertices_by_color(
    unsigned int color_id) const;
  std::vector<const std::vector<double>*> get_vertices_by_ids(
    unsigned int color_id,
    int wheel_weapon_id) const;

  void move_model_to_point(const std::vector<double> &point);
  void move_model_to_point_inv_neg_vol(const std::vector<double> &point);
  void move_model_to_point(
    std::vector<const std::vector<double>*> vertices,
    const std::vector<double> &point);
  void move_model_to_point(
    std::vector<const face*> polygons,
    const std::vector<double> &point);

  void move_coord_system_to_point(const std::vector<double> &point);
  void move_coord_system_to_point_inv_neg_vol(
    const std::vector<double> &point);
  void move_coord_system_to_center();
  void move_coord_system_to_point(
    std::vector<const std::vector<double>*> vertices,
    const std::vector<double> &point);
  void move_coord_system_to_point(
    std::vector<const face*> polygons,
    const std::vector<double> &point);

  void rotate_by_axis(double angle, rotation_axis axis);

  void set_color_id(unsigned int new_color_id, int new_wheel_weapon_num = -1);


  bool find_ref_points();


  void calculate_rmax();
  void calculate_c3d_properties();


  std::vector<std::vector<std::vector<double>>>
    get_planes_4_extreme_points() const;
  static std::vector<std::unordered_map<std::size_t, double>>
    get_verts_plane_lengths_rel_points(
      std::size_t perpendicular_axis,
      const std::vector<std::vector<double>> &verts_arg,
      const std::vector<std::vector<double>> &points_2d,
      const generate_bound::layer_vert_inds &vert_inds);
  static generate_bound::layer_vert_inds get_min_length_layer_points(
    const std::vector<std::unordered_map<std::size_t, double>> &
      verts_plane_lengths_rel_points);
  static double get_plane_area_from_points(
    std::size_t perpendicular_axis,
    const std::vector<std::vector<double>> &verts_arg,
    const generate_bound::layer_vert_inds &vert_inds);
  static std::vector<std::vector<double>> get_extr_middle_points(
    std::size_t perpendicular_axis,
    const std::vector<std::vector<double>> &verts_arg,
    const generate_bound::layer_vert_inds &layer_extrs);
  polyhedron extr_inds_to_bound(
    const std::vector<std::vector<double>> &verts_arg,
    const generate_bound::layers_inds_of_axis &extr_inds,
    const generate_bound::layers_inds_of_axis &middle_inds,
    generate_bound::model_type type,
    const model_extreme_points *wheel_params_extremes = nullptr) const;
  polyhedron generate_bound_model(
    const generate_bound::model_type type,
    const std::size_t layers_num,
    const double area_threshold_multiplier,
    const model_extreme_points *wheel_params_extremes = nullptr) const;

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


  std::vector<double> &offset_point();
  const std::vector<double> &offset_point() const;

  double x_off() const;
  double y_off() const;
  double z_off() const;

  void set_x_off(double new_x_off);
  void set_y_off(double new_y_off);
  void set_z_off(double new_z_off);


  int numVerts, numVertNorms, numFaces, numVertTotal, numVertsPerPoly;
  model_extreme_points extreme_points;
  model_offset offset;
  double rmax;
  double volume;
  std::vector<double> rcm;
  std::vector<std::vector<double>> J;
  int bodyColorOffset, bodyColorShift;
//  double verts[MAX_VERTS][3];
  std::vector<std::vector<double>> verts;
  std::vector<std::vector<double>> vertNorms;
//  FACE faces[MAX_FACES];
  std::vector<face> faces;



  std::pair<int, int> ref_vert_one_ind;
  const std::vector<double> *ref_vert_one;
  std::pair<int, int> ref_vert_two_ind;
  const std::vector<double> *ref_vert_two;
  std::pair<int, int> ref_vert_three_ind;
  const std::vector<double> *ref_vert_three;

  std::vector<double> ref_vert_two_rel_to_one;
  std::vector<double> ref_vert_three_rel_to_one;

  double ref_angle;

  std::unordered_set<std::size_t> wheels;
  std::unordered_set<std::size_t> wheels_steer;
  std::unordered_set<std::size_t> wheels_non_steer;
  std::unordered_set<std::size_t> wheels_ghost;
  std::unordered_set<std::size_t> wheels_non_ghost;

  // Holds wheel id in case model itself is a wheel.
  // Otherwise value is less than 0.
  int wheel_id;

  // Used only when converting from *.obj to *.m3d/*.a3d.
  std::string wavefront_obj_path;

  // Used only when converting from *.m3d to *.obj.
  bool volume_overwritten;
  bool rcm_overwritten;
  bool J_overwritten;
} POLYHEDRON;


/*
   ============================================================================
   globals
   ============================================================================
*/

static int A;   /* alpha */
static int B;   /* beta */
static int C;   /* gamma */

/* projection integrals */
static double P1, Pa, Pb, Paa, Pab, Pbb, Paaa, Paab, Pabb, Pbbb;

/* face integrals */
static double Fa, Fb, Fc, Faa, Fbb, Fcc, Faaa, Fbbb, Fccc, Faab, Fbbc, Fcca;

/* volume integrals */
static double T0, T1[3], T2[3], TP[3];



/*
   ============================================================================
   read in a polyhedron
   ============================================================================
*/

//void readPolyhedron(char *name, POLYHEDRON *p);

/*
   ============================================================================
   compute mass properties
   ============================================================================
*/


/* compute various integrations over projection of face */
void compProjectionIntegrals(POLYHEDRON *p, FACE *f);

void compFaceIntegrals(FACE *f);

void compVolumeIntegrals(POLYHEDRON *p);


/*
   ============================================================================
   main
   ============================================================================
*/


// int main(int argc, char *argv[]);



} // namespace volInt

#endif // VOLINT_H
