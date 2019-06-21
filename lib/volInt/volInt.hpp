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



#include <stdexcept>

// TEST
//#include <iostream>
#include <algorithm>
#include <cmath>
#include <utility>
#include <limits>
#include <vector>
#include <unordered_set>
#include <string>
#include <functional>



#include <stdio.h>
#include <string.h>
#include <math.h>

// TEST
//#include <cstdlib>
//#include <signal.h>



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



enum class rotation_axis{x, y, z};



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

// not needed
//std::vector<double> vector_scale_to_max_coord(
//  double max_coord,
//  const std::vector<double> &vec);
//void vector_scale_self_to_max_coord(
//  double max_coord,
//  const std::vector<double> &vec);

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

void matrix_multiply_self(
  std::vector<std::vector<double>> &mat,
  double num);


template<typename T>
void get_groups_of_connected_items_helper_check_item_in_group(
  T cur_item,
  const std::vector<std::vector<T>> &groups,
  bool &cur_item_in_group,
  std::size_t &cur_item_group_num)
{
  if(!groups.empty())
  {
    for(std::size_t cur_group_num = 0,
          groups_size = groups.size();
        cur_group_num < groups_size;
        ++cur_group_num)
    {
      if(std::find(groups[cur_group_num].begin(),
                   groups[cur_group_num].end(),
                   cur_item) !=
         groups[cur_group_num].end())
      {
        cur_item_in_group = true;
        cur_item_group_num = cur_group_num;
        break;
      }
    }
  }
}



template<typename T>
std::vector<std::vector<T>> get_groups_of_connected_items(
  std::vector<T> orig_vec,
  std::function<bool(T first, T second)> check_connected_func)
{
  std::vector<std::vector<T>> vecs_to_return;
  std::vector<std::vector<T>> groups;
  std::size_t orig_vec_size = orig_vec.size();
  groups.reserve(orig_vec_size);

  std::size_t non_empty_groups_num = 0;

  for(std::size_t cur_item_num = 0;
      cur_item_num < orig_vec_size;
      ++cur_item_num)
  {
    T cur_item = orig_vec[cur_item_num];

    bool cur_item_isolated = true;

    // Checking if cur_item is already in group.
    bool cur_item_in_group = false;
    std::size_t cur_item_group_num;
    get_groups_of_connected_items_helper_check_item_in_group<T>(
      cur_item,
      groups,
      cur_item_in_group,
      cur_item_group_num);

    // Checking all elements of orig_vec for connection with cur_item.
    for(std::size_t item_to_compare_num = 0;
        item_to_compare_num < orig_vec_size;
        ++item_to_compare_num)
    {
      T item_to_compare = orig_vec[item_to_compare_num];
      // Skipping all operations if cur_item is item_to_compare.
      if(cur_item_num == item_to_compare_num)
      {
        continue;
      }

      // Skipping if items are not connected.
      if(!check_connected_func(cur_item, item_to_compare))
      {
        continue;
      }

      cur_item_isolated = false;



      // Checking if item_to_compare is already in group.
      bool item_to_compare_in_group = false;
      std::size_t item_to_compare_group_num;
      get_groups_of_connected_items_helper_check_item_in_group<T>(
        item_to_compare,
        groups,
        item_to_compare_in_group,
        item_to_compare_group_num);

      // If both connected items are already in groups
      // but those groups are different.
      // Inserting elements of item_to_compare group into cur_item group.
      if(cur_item_in_group &&
         item_to_compare_in_group &&
         cur_item_group_num != item_to_compare_group_num)
      {
        // TEST
        //std::cout << "case 1" << '\n';
        groups[cur_item_group_num].insert(
          groups[cur_item_group_num].end(),
          groups[item_to_compare_group_num].begin(),
          groups[item_to_compare_group_num].end());
        // Clearing 2nd group and never reusing it again.
        groups[item_to_compare_group_num].clear();
        groups[item_to_compare_group_num].shrink_to_fit();

        --non_empty_groups_num;
      }
      // If only 1 item is in group other item is inserted in this group.
      else if(cur_item_in_group && !item_to_compare_in_group)
      {
        // TEST
        //std::cout << "case 2" << '\n';
        groups[cur_item_group_num].push_back(item_to_compare);
      }
      else if(!cur_item_in_group && item_to_compare_in_group)
      {
        // TEST
        //std::cout << "case 3" << '\n';
        // Note that cur_item_in_group and cur_item_group_num are changed
        // since they are later used in this loop.
        cur_item_in_group = true;
        cur_item_group_num = item_to_compare_group_num;
        groups[item_to_compare_group_num].push_back(cur_item);
      }
      // If both items are not in group creating new one.
      else if(!cur_item_in_group && !item_to_compare_in_group)
      {
        // TEST
        //std::cout << "case 4" << '\n';
        std::size_t last_group_num = groups.size();

        cur_item_in_group = true;
        cur_item_group_num = last_group_num;

        groups.push_back(std::vector<T>());
        groups[cur_item_group_num].reserve(orig_vec_size);
        groups[cur_item_group_num].push_back(cur_item);
        groups[cur_item_group_num].push_back(item_to_compare);

        ++non_empty_groups_num;
      }
    }

    // If cur_item is not connected with any other items
    // create new group for this item.
    if(cur_item_isolated)
    {
      // TEST
      //std::cout << "case 5" << '\n';
      groups.push_back(std::vector<T>(1, cur_item));

      ++non_empty_groups_num;
    }
  }

  // Getting rid of unused groups.
  vecs_to_return.reserve(non_empty_groups_num);
  for(auto &&group : groups)
  {
    if(!group.empty())
    {
      vecs_to_return.push_back(std::move(group));
    }
  }
  vecs_to_return.shrink_to_fit();

  return vecs_to_return;
}



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

  double check_volume();

  void get_extreme_points();
  model_extreme_points get_extreme_points(
    const std::vector<const volInt::face*> &polygons) const;


  std::vector<double> get_model_center();
  std::vector<double> get_model_center(
    const std::vector<const std::vector<double>*> &vertices) const;
  std::vector<double> get_model_center(
    const std::vector<const volInt::face*> &polygons) const;

  std::vector<const volInt::face*>
    get_polygons_by_color(unsigned int color_id) const;
  std::vector<const volInt::face*> get_polygons_by_ids(
    unsigned int color_id,
    int wheel_weapon_id) const;

  std::vector<const std::vector<double>*> get_vertices_by_polygons(
    const std::vector<const volInt::face*> &model_polygons) const;

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
    std::vector<const volInt::face*> polygons,
    const std::vector<double> &point);

  void move_coord_system_to_point(const std::vector<double> &point);
  void move_coord_system_to_point_inv_neg_vol(
    const std::vector<double> &point);
  void move_coord_system_to_center();
  void move_coord_system_to_point(
    std::vector<const std::vector<double>*> vertices,
    const std::vector<double> &point);
  void move_coord_system_to_point(
    std::vector<const volInt::face*> polygons,
    const std::vector<double> &point);

  void rotate_by_axis(double angle, rotation_axis axis);


  bool find_ref_points();


  void calculate_rmax();
  void calculate_c3d_properties();

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

  // Used only when converting from *.obj to *.m3d/*.a3d.
  std::string wavefront_obj_path;
  std::unordered_set<int> wheels;
  std::unordered_set<int> wheels_steer;
  std::unordered_set<int> wheels_non_steer;
  std::unordered_set<int> wheels_ghost;
  std::unordered_set<int> wheels_non_ghost; // Used when converting
                                            // from *.m3d to *.obj.
  int wheel_id; // Holds wheel id in case model itself is a wheel.
                // Otherwise value is less than 0.

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
