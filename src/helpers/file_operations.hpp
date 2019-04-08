#ifndef TRACTOR_CONVERTER_FILE_OPERATIONS_H
#define TRACTOR_CONVERTER_FILE_OPERATIONS_H

#include "defines.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <exception>
#include <stdexcept>

#include <string>

#define TRACTOR_CONVERTER_FILE_READ_ALL -1
#define TRACTOR_CONVERTER_FILE_WRITE_ALL -1

#define TRACTOR_CONVERTER_FILE_OVERWRITE true
#define TRACTOR_CONVERTER_FILE_INSERT false

#define TRACTOR_CONVERTER_BINARY true
#define TRACTOR_CONVERTER_NON_BINARY false

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



const std::size_t read_buffer_size = 4096;



std::string read_file(boost::filesystem::ifstream &file,
                      const bool binary,
                      const int start_byte_string_num,
                      const int start_byte_file_num,
                      const int bytes_to_read_num_arg,
                      const std::string &file_name_error);
std::string read_file(const std::string &path_string,
                      const bool binary,
                      const int start_byte_string_num,
                      const int start_byte_file_num,
                      const int bytes_to_read_num_arg,
                      const std::string &file_name_error);
std::string read_file(const boost::filesystem::path &path,
                      const bool binary,
                      const int start_byte_string_num,
                      const int start_byte_file_num,
                      const int bytes_to_read_num_arg,
                      const std::string &file_name_error);



// overwrites part of file instead of appending
void write_to_file(boost::filesystem::ofstream &file,
                   const std::string &bytes_to_write,
                   const bool binary,
                   const int start_byte_string_num,
                   const int start_byte_file_num,
                   const int bytes_to_write_num_arg,
                   const std::string &file_name_error);
void write_to_file(const std::string &path_string,
                   const std::string &bytes_to_write,
                   const bool overwrite,
                   const bool binary,
                   const int start_byte_string_num,
                   const int start_byte_file_num,
                   const int bytes_to_write_num_arg,
                   const std::string &file_name_error);
void write_to_file(const boost::filesystem::path &path,
                   const std::string &bytes_to_write,
                   const bool overwrite,
                   const bool binary,
                   const int start_byte_string_num,
                   const int start_byte_file_num,
                   const int bytes_to_write_num_arg,
                   const std::string &file_name_error);

void save_file(const std::string &path_string,
               const std::string &bytes_to_write,
               const bool binary,
               const std::string &file_name_error);
void save_file(const boost::filesystem::path &path,
               const std::string &bytes_to_write,
               const bool binary,
               const std::string &file_name_error);



boost::filesystem::path get_directory(const std::string &path_string,
                                      const std::string &dir_name_error);

} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_FILE_OPERATIONS_H
