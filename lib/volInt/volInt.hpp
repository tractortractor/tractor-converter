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

#include <algorithm>
#include <cmath>
#include <utility>
#include <limits>
#include <vector>
#include <unordered_set>
#include <string>



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
} // namespace exception



std::vector<double> normalize(double norm, const std::vector<double> &vec);

std::vector<double> vector_minus(
  const std::vector<double> &first,
  const std::vector<double> &second);

double vector_dot_product(
  const std::vector<double> &first,
  const std::vector<double> &second);

std::vector<double> vector_cross_product(
  const std::vector<double> &first,
  const std::vector<double> &second);

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



const int min_float_precision = 3;
const double ref_points_distinct_distance = 1.0/std::pow(10, min_float_precision);
//const double ref_points_distinct_distance = 1.0e-3;
const double sqr_ref_points_distinct_distance =
  VOLINT_SQR(ref_points_distinct_distance);
const double density = 1.0;

/*
   ============================================================================
   data structures
   ============================================================================
*/

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

  double check_volume();
  void invertVertNorms();
  void reverse_polygons_orientation();
  void faces_calc_params(); // Must be called again if model was moved.

  std::pair<std::vector<double>, std::vector<double>> get_extreme_points();


  std::vector<double> get_model_center();
  void move_model_to_point(const std::vector<double> &point_coords);
  void move_coord_system_to_point(const std::vector<double> &point_coords);


  bool find_ref_points();


  void calculate_rmax();
  void calculate_c3d_properties();

  int numVerts, numVertNorms, numFaces, numVertTotal, numVertsPerPoly;
  double xmax, ymax, zmax;
  double xmin, ymin, zmin;
  double x_off, y_off, z_off;
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
