#ifndef TRACTOR_CONVERTER_USAGE_PAL_H
#define TRACTOR_CONVERTER_USAGE_PAL_H

#include "defines.hpp"
#include "tga_constants.hpp"

#include "hex.hpp"
#include "check_option.hpp"
#include "file_operations.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <exception>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>



namespace tractor_converter{



void usage_pal_mode_save_output_readable(
  boost::filesystem::path file,
  const std::vector<int> &used_characters,
  const std::string &file_name_error);
void usage_pal_mode_save_output_not_readable(
  boost::filesystem::path file,
  const std::vector<int> &used_characters,
  const std::string &file_name_error);
void usage_pal_mode_save_output(
  boost::filesystem::path file,
  const std::vector<int> &used_characters,
  bool readable_output,
  const std::string &file_name_error);
void usage_pal_mode(const boost::program_options::variables_map options);



} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_USAGE_PAL_H
