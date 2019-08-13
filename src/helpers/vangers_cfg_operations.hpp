#ifndef TRACTOR_CONVERTER_VANGERS_CFG_OPERATIONS_H
#define TRACTOR_CONVERTER_VANGERS_CFG_OPERATIONS_H

#include "defines.hpp"

#include "raw_num_operations.hpp"
#include "to_string_precision.hpp"

#include <boost/algorithm/string.hpp>

#include <zlib.h>

#include <exception>
#include <stdexcept>

#include <cstdlib>
#include <cwctype>
#include <cstring>
#include <string>



namespace tractor_converter{
namespace helpers{



const char win_dir_separator = '\\';
const char unix_dir_separator = '/';



namespace van_cfg_key{

  namespace game_lst{
    const std::string NumModel =            "NumModel";
    const std::string MaxSize =             "MaxSize";
    const std::string ModelNum =            "ModelNum";
    const std::string Name =                "Name";
    const std::string Size =                "Size";
    const std::string NameID =              "NameID";

    const std::string NameFireBallProcess = "NameFireBallProcess";
    const std::string NameDeformProcess =   "NameDeformProcess";


    const std::vector<std::string> path_vars =
      {
        Name,
        NameFireBallProcess,
        NameDeformProcess,
      };
  } // namespace game_lst

  namespace prm{
    const std::string delimiter =  ":";
    const std::string scale_size = "scale_size" + delimiter;
  } // namespace prm

} // namespace van_cfg_key



namespace exception{
  class raw_uncompress_error : public virtual std::exception
  {
    public:
    raw_uncompress_error(const char *err_msg, int zlib_err_code);
    const char* what() const noexcept;
    int zlib_err_code() const noexcept;

    private:
    std::string m_zlib_string;
    int m_zlib_err_code;
  };
} // namespace exception



std::string raw_uncompress(std::size_t decompressed_size,
                           const std::string &compressed);



namespace xzip_crypt{
  const std::size_t key_pos = sizeof(char);
  const std::size_t enc_beg_pos = key_pos + sizeof(std::uint32_t);

  namespace key{
    const unsigned int multiplier = 6386891;
    const unsigned int bin_or = 1;
  } // namespace key

  unsigned int crt(unsigned int &val);
} // namespace xzip_crypt



namespace xzip_decompress {
  const std::size_t label_pos = 0;
  const std::size_t decomp_size_pos = sizeof(std::int16_t);
  const std::size_t comp_beg_pos = decomp_size_pos + sizeof(std::uint32_t);

  const std::size_t add_decomp_size = 12;
} // namespace xzip_decompress



namespace sicher_cfg_format{
  const std::string sprintf_int = "%i";
  const int sprintf_float_precision = 6;
  const std::string sprintf_float =
    "%." + std::to_string(sprintf_float_precision) + "g";
} // namespace sicher_cfg_format



std::string fix_game_lst_path(std::string path);



// To read Sicher's *.prm and *.lst files.
class sicher_cfg_reader
{

public:

  sicher_cfg_reader(std::string &&str_arg,
                    const std::string &input_file_path_str_arg,
                    const std::string &input_file_name_error_arg);

  template<typename T>
  T get_next_value(const std::string &value_name)
  {
    move_pos_to_value(value_name);
    return get_cur_pos_value_str<T>();
  }

  template<typename T>
  T get_next_value_keep_pos(const std::string &value_name)
  {
    char *orig_pos = pos;
    T ret_value = get_next_value<T>(value_name);
    pos = orig_pos;
    return ret_value;
  }

  bool check_next_value(const std::string &value_name);

  void fix_game_lst_paths();

  const std::string &str();
  std::string &&extract_str();

protected:

  std::string m_str;
  char *pos;
  const char *end_pos;
  std::string input_file_path_str;
  std::string input_file_name_error;

  char *get_pos_of_next_value(const std::string &value_name);
  // Set pos pointer right after value_name string found in m_str.
  void move_pos_to_value(const std::string &value_name);

  template<typename T>
  void skip_spaces_until_value()
  {
    for(; pos < end_pos; ++pos)
    {
      if(!std::isspace(*pos))
      {
        break;
      }
    }
  }

  void move_pos_after_str_value();

  template<typename T>
  T get_cur_pos_value_str();

private:

  void decrypt();
  void decompress();

};



// To create new Sicher's *.prm and *.lst files from existing ones.
class sicher_cfg_writer : public sicher_cfg_reader
{

public:

  sicher_cfg_writer(std::string &&str_arg,
                    const std::string &input_file_path_str_arg,
                    const std::string &input_file_name_error_arg,
                    std::size_t size_increase_arg);

  void write_until_end();

  template<typename T>
  void overwrite_next_value(const std::string &value_name,
                            const T &value,
                            const std::string &format =
                              sicher_cfg_format::sprintf_float)
  {
    move_pos_to_value(value_name);
    overwrite_cur_pos_value(value, format);
  }

  const std::string &out_str();
  std::string &&extract_out_str();

protected:

  std::string m_out_str;
  // Points to first character in m_str which was not copied to m_out_str.
  char *non_copied_pos;

private:

  void write_until_pos();

  template<typename T>
  void append_value(const T &value, const std::string &format);

  template<typename T>
  void move_pos_after_value();

  template<typename T>
  void overwrite_cur_pos_value(const T &value, const std::string &format)
  {
    skip_spaces_until_value<T>();
    write_until_pos();

    append_value<T>(value, format);

    move_pos_after_value<T>();
    non_copied_pos = pos;
  }

};



} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_VANGERS_CFG_OPERATIONS_H
