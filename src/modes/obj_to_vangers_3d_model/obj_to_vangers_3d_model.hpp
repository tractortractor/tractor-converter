#ifndef TRACTOR_CONVERTER_OBJ_TO_VANGERS_3D_MODEL_H
#define TRACTOR_CONVERTER_OBJ_TO_VANGERS_3D_MODEL_H

#include "defines.hpp"
#include "bitflag.hpp"
#include "vangers_3d_model_constants.hpp"

#include "hex.hpp"
#include "check_option.hpp"
#include "get_option.hpp"
#include "file_operations.hpp"
#include "vangers_3d_model_operations.hpp"
#include "wavefront_obj_to_m3d_operations.hpp"

#include "tiny_obj_loader.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <exception>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

namespace tractor_converter{

void obj_to_vangers_3d_model_mode(
  const boost::program_options::variables_map options);

} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_OBJ_TO_VANGERS_3D_MODEL_H
