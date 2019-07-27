INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)



set(BOOST_MIN_VERSION "1.60.0")

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
if(WIN32)
  add_definitions(-DBOOST_ALL_NO_LIB)
  # Must be used or MSVC 2017 will link program_options AND libprogram_options
  # even when asked to link ONLY program_options.
  add_definitions(-DBOOST_ALL_DYN_LINK)
endif(WIN32)
FIND_PACKAGE(Boost ${BOOST_MIN_VERSION} REQUIRED
  system
  filesystem
  program_options
  )

#SET(BOOST_LIBS_TO_USE ${Boost_LIBRARIES})


FIND_PACKAGE(ZLIB REQUIRED)



SET(TRACTOR_CONVERTER_SOURCES

  modes/usage_pal/usage_pal.cpp
  modes/remove_not_used_pal/remove_not_used_pal.cpp
  modes/tga_merge_unused_pal/tga_merge_unused_pal.cpp
  modes/tga_replace_pal/tga_replace_pal.cpp
  modes/extract_tga_pal/extract_tga_pal.cpp
  modes/vangers_pal_to_tga_pal/vangers_pal_to_tga_pal.cpp
  modes/pal_shift_for_vangers_avi/pal_shift_for_vangers_avi.cpp
  modes/compare_bmp_escave_outside/compare_bmp_escave_outside.cpp
  modes/tga_to_bmp/tga_to_bmp.cpp
  modes/bmp_to_tga/bmp_to_tga.cpp

  modes/vangers_3d_model_to_obj/vangers_3d_model_to_obj.cpp
  modes/obj_to_vangers_3d_model/obj_to_vangers_3d_model.cpp
  modes/create_wavefront_mtl/create_wavefront_mtl.cpp
  modes/create_materials_table/create_materials_table.cpp


  get_options/get_options.cpp

  helpers/wavefront_obj_to_m3d_operations.cpp
  helpers/m3d_to_wavefront_obj_operations.cpp
  helpers/vangers_3d_model_operations.cpp
  helpers/wavefront_obj_operations.cpp
  helpers/vangers_cfg_operations.cpp
  helpers/check_pal_color_used.cpp
  helpers/tga_class.cpp
  helpers/to_string_precision.cpp
  helpers/file_operations.cpp
  helpers/parse_mtl_body_offs.cpp
  helpers/get_option.cpp
  helpers/check_option.cpp
  helpers/raw_num_operations.cpp
  helpers/bitflag.cpp
  helpers/hex.cpp

  main/main.cpp

  )

SET(TRACTOR_CONVERTER_HEADER_FILES

  modes/usage_pal/usage_pal.hpp
  modes/remove_not_used_pal/remove_not_used_pal.hpp
  modes/tga_merge_unused_pal/tga_merge_unused_pal.hpp
  modes/tga_replace_pal/tga_replace_pal.hpp
  modes/extract_tga_pal/extract_tga_pal.hpp
  modes/vangers_pal_to_tga_pal/vangers_pal_to_tga_pal.hpp
  modes/pal_shift_for_vangers_avi/pal_shift_for_vangers_avi.hpp
  modes/compare_bmp_escave_outside/compare_bmp_escave_outside.hpp
  modes/tga_to_bmp/tga_to_bmp.hpp
  modes/bmp_to_tga/bmp_to_tga.hpp

  modes/vangers_3d_model_to_obj/vangers_3d_model_to_obj.hpp
  modes/obj_to_vangers_3d_model/obj_to_vangers_3d_model.hpp
  modes/create_wavefront_mtl/create_wavefront_mtl.hpp
  modes/create_materials_table/create_materials_table.hpp


  get_options/get_options.hpp

  helpers/wavefront_obj_to_m3d_operations.hpp
  helpers/m3d_to_wavefront_obj_operations.hpp
  helpers/vangers_3d_model_operations.hpp
  helpers/wavefront_obj_operations.hpp
  helpers/vangers_cfg_operations.hpp
  helpers/check_pal_color_used.hpp
  helpers/tga_class.hpp
  helpers/to_string_precision.hpp
  helpers/file_operations.hpp
  helpers/parse_mtl_body_offs.hpp
  helpers/get_option.hpp
  helpers/check_option.hpp
  helpers/raw_num_operations.hpp
  helpers/bitflag.hpp
  helpers/hex.hpp

  helpers/vangers_3d_model_constants.hpp
  helpers/wavefront_obj_constants.hpp
  helpers/tga_constants.hpp

  ../lib/volInt/volInt.hpp
  ../lib/alphanum/alphanum.hpp

  main/defines.hpp
  main/main.hpp

  )



add_executable(tractor_converter
  ${TRACTOR_CONVERTER_SOURCES}
  ${TRACTOR_CONVERTER_HEADER_FILES}
  )



# include dirs
target_include_directories(tractor_converter SYSTEM
  PRIVATE ${Boost_INCLUDE_DIRS}
  PRIVATE ${ZLIB_INCLUDE_DIR}
  )

target_include_directories(tractor_converter

  PUBLIC main
  PUBLIC helpers
  PUBLIC get_options


  PUBLIC modes/usage_pal
  PUBLIC modes/remove_not_used_pal
  PUBLIC modes/tga_merge_unused_pal
  PUBLIC modes/tga_replace_pal
  PUBLIC modes/extract_tga_pal
  PUBLIC modes/vangers_pal_to_tga_pal
  PUBLIC modes/pal_shift_for_vangers_avi
  PUBLIC modes/compare_bmp_escave_outside
  PUBLIC modes/tga_to_bmp
  PUBLIC modes/bmp_to_tga

  PUBLIC modes/vangers_3d_model_to_obj
  PUBLIC modes/obj_to_vangers_3d_model
  PUBLIC modes/create_wavefront_mtl
  PUBLIC modes/create_materials_table


  PUBLIC ../lib/volInt
  PUBLIC ../lib/alphanum
  PUBLIC ../lib/tinyobjloader
  )

# libraries linking
target_link_libraries(tractor_converter PUBLIC
  ${Boost_LIBRARIES}
  ${ZLIB_LIBRARIES}
  volInt
  alphanum
  tinyobjloader
  )



INSTALL(TARGETS tractor_converter RUNTIME DESTINATION ./)

