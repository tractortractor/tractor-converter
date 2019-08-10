#ifndef TRACTOR_CONVERTER_MAIN_H
#define TRACTOR_CONVERTER_MAIN_H

#include "defines.hpp"

#include "get_options.hpp"

#include "usage_pal.hpp"
#include "remove_not_used_pal.hpp"
#include "tga_merge_unused_pal.hpp"
#include "tga_replace_pal.hpp"
#include "extract_tga_pal.hpp"
#include "vangers_pal_to_tga_pal.hpp"
#include "pal_shift_for_vangers_avi.hpp"
#include "compare_bmp_escave_outside.hpp"
#include "bmp_to_tga.hpp"
#include "tga_to_bmp.hpp"

#include "vangers_3d_model_to_obj.hpp"
#include "obj_to_vangers_3d_model.hpp"
#include "create_wavefront_mtl.hpp"
#include "create_materials_table.hpp"


#include <boost/static_assert.hpp>
#include <boost/predef/other/endian.h>
//#include <boost/detail/endian.hpp>
//#include <boost/endian/endian.hpp>
//#include <boost/boost/predef/detail/endian_compat.h>

#include <boost/program_options.hpp>

#include <exception>
#include <stdexcept>

#include <climits>
#include <string>


#if CHAR_BIT != 8
#error Platforms with CHAR_BIT != 8 are not supported
#endif

#if !defined(BOOST_ENDIAN_LITTLE_BYTE)
#error Target architecture must be little-endian
#endif

BOOST_STATIC_ASSERT_MSG(
  std::numeric_limits<double>::is_iec559,
  "Floating point numbers must fulfill the requirements "
    "of IEC 559 (IEEE 754) standard");

BOOST_STATIC_ASSERT_MSG(
  std::numeric_limits<float>::digits == 24,
  "float must be 32-bit with 24-bit mantissa.");

BOOST_STATIC_ASSERT_MSG(
  std::numeric_limits<double>::digits == 53,
  "double must be 64-bit with 53-bit mantissa.");

// Because of volInt::calc_norms::normal_to_key.
BOOST_STATIC_ASSERT_MSG(
  std::numeric_limits<std::size_t>::digits >= 33,
  "std::size_t must be at least 33-bit.");

#endif // TRACTOR_CONVERTER_MAIN_H
