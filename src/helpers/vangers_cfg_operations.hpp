#ifndef TRACTOR_CONVERTER_VANGERS_CFG_OPERATIONS_H
#define TRACTOR_CONVERTER_VANGERS_CFG_OPERATIONS_H

#include "defines.hpp"

#include "to_string_precision.hpp"

#include <exception>
#include <stdexcept>

#include <cstdlib>
#include <cwctype>
#include <cstring>
#include <string>
//#include <vector>

namespace tractor_converter{
namespace helpers{



const std::string sprintf_int_sicher_cfg_format = "%i";

const int sprintf_float_sicher_cfg_format_precision = 6;

const std::string sprintf_float_sicher_cfg_format =
  "%." + std::to_string(sprintf_float_sicher_cfg_format_precision) + "g";


//const std::string sprintf_float_sicher_cfg_format =
//"%." + std::to_string(float_precision_sicher_cfg) + "e";



// To read sicher's *.prm and *.lst files.
class sicher_cfg_reader
{

public:

  sicher_cfg_reader(std::string &&str_arg,
                    const std::string &input_file_path_str_arg,
                    const std::string &input_file_name_error_arg);

  // Set pos pointer right after value_name string found in m_str.
  void move_pos_to_value(const std::string &value_name);
  template<typename T>
  T get_next_value(const std::string &value_name);

  const std::string &str();

protected:

  std::string m_str;
  char *pos;
  const char *end_pos;
  std::string input_file_path_str;
  std::string input_file_name_error;

private:

  template<typename T>
  T get_next_value_helper_convert_str();

};



// to create new prm file from existing one
class sicher_cfg_writer : public sicher_cfg_reader
{

public:

  sicher_cfg_writer(std::string &&str_arg,
                    const std::string &input_file_path_str_arg,
                    const std::string &input_file_name_error_arg,
                    std::size_t size_increase);

  void write_until_pos();
  void write_until_end();
  template<typename T>
  void overwrite_next_value(const std::string &value_name,
                            T value,
                            const std::string &format);

  const std::string &out_str();

protected:

  std::string m_out_str;
  char *copied_pos;

private:

  template<typename T>
  void overwrite_next_value_helper_append_value(const T &value,
                                                const std::string &format);

  template<typename T>
  void overwrite_next_value_helper_move_pos_after_value();

};



} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_VANGERS_CFG_OPERATIONS_H
