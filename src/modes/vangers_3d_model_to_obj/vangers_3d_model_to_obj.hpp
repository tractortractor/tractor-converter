#ifndef TRACTOR_CONVERTER_VANGERS_3D_MODEL_TO_OBJ_H
#define TRACTOR_CONVERTER_VANGERS_3D_MODEL_TO_OBJ_H

#include "defines.hpp"
#include "bitflag.hpp"
#include "vangers_3d_model_constants.hpp"

#include "hex.hpp"
#include "check_option.hpp"
#include "file_operations.hpp"
#include "vangers_3d_model_operations.hpp"
#include "m3d_to_wavefront_obj_operations.hpp"

#include "tiny_obj_loader.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <exception>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>



namespace tractor_converter{



enum class scale_from_map_type{mechos, non_mechos};
double scale_from_map(
  const std::unordered_map<std::string, double> &scale_sizes,
  const boost::filesystem::path &to_lookup_path,
  const boost::filesystem::path &input_game_dir_root,
  const std::string &input_file_name_error,
  scale_from_map_type type,
  double default_scale);

void vangers_3d_model_to_obj_mode(
  const boost::program_options::variables_map options);



} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_VANGERS_3D_MODEL_TO_OBJ_H
