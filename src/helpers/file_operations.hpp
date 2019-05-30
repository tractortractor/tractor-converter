#ifndef TRACTOR_CONVERTER_FILE_OPERATIONS_H
#define TRACTOR_CONVERTER_FILE_OPERATIONS_H

#include "defines.hpp"
#include "bitflag.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <exception>
#include <stdexcept>

#include <string>

namespace tractor_converter{
namespace helpers{



namespace exception{
  struct file_not_found : public virtual std::runtime_error
  {
    using std::runtime_error::runtime_error;
  };

  struct file_not_saved : public virtual std::runtime_error
  {
    using std::runtime_error::runtime_error;
  };

  struct directory_not_found : public virtual std::runtime_error
  {
    using std::runtime_error::runtime_error;
  };
} // namespace exception



const int write_all_dummy_size = -1;
const int read_all_dummy_size = -1;

enum class file_flag
{
  none = 0,
  read_all = 1,
  write_all = 2,
  overwrite = 3,
  binary = 4,
};



const std::size_t read_buffer_size = 4096;



std::string read_file(boost::filesystem::ifstream &file,
                      const bitflag<file_flag> flags,
                      const int start_byte_string_num,
                      const int start_byte_file_num,
                      const int bytes_to_read_num_arg,
                      const std::string &file_name_error);
std::string read_file(const boost::filesystem::path &path,
                      const bitflag<file_flag> flags,
                      const int start_byte_string_num,
                      const int start_byte_file_num,
                      const int bytes_to_read_num_arg,
                      const std::string &file_name_error);
std::string read_file(const std::string &path_string,
                      const bitflag<file_flag> flags,
                      const int start_byte_string_num,
                      const int start_byte_file_num,
                      const int bytes_to_read_num_arg,
                      const std::string &file_name_error);



// overwrites part of file instead of appending
void write_to_file(boost::filesystem::ofstream &file,
                   const std::string &bytes_to_write,
                   const bitflag<file_flag> flags,
                   const int start_byte_string_num,
                   const int start_byte_file_num,
                   const int bytes_to_write_num_arg,
                   const std::string &file_name_error);
void write_to_file(const boost::filesystem::path &path,
                   const std::string &bytes_to_write,
                   const bitflag<file_flag> flags,
                   const int start_byte_string_num,
                   const int start_byte_file_num,
                   const int bytes_to_write_num_arg,
                   const std::string &file_name_error);
void write_to_file(const std::string &path_string,
                   const std::string &bytes_to_write,
                   const bitflag<file_flag> flags,
                   const int start_byte_string_num,
                   const int start_byte_file_num,
                   const int bytes_to_write_num_arg,
                   const std::string &file_name_error);

void save_file(const boost::filesystem::path &path,
               const std::string &bytes_to_write,
               const bitflag<file_flag> flags,
               const std::string &file_name_error);
void save_file(const std::string &path_string,
               const std::string &bytes_to_write,
               const bitflag<file_flag> flags,
               const std::string &file_name_error);



boost::filesystem::path get_directory(const std::string &path_string,
                                      const std::string &dir_name_error);

} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_FILE_OPERATIONS_H
