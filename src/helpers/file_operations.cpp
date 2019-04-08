#include "file_operations.hpp"



namespace tractor_converter{
namespace helpers{



std::string read_file(boost::filesystem::ifstream &file,
                      const bool binary,
                      const int start_byte_string_num,
                      const int start_byte_file_num,
                      const int bytes_to_read_num_arg,
                      const std::string &file_name_error)
{
  std::string bytes_to_return;
  std::streamoff bytes_to_read_num;

  file.seekg(start_byte_file_num, std::ios::end);
  std::streamoff expected_file_size = file.tellg();
  file.seekg(0, std::ios::beg);

  if(binary == TRACTOR_CONVERTER_BINARY)
  {
    if(bytes_to_read_num_arg == TRACTOR_CONVERTER_FILE_READ_ALL)
    {
      bytes_to_read_num = expected_file_size;
    }
    else
    {
      bytes_to_read_num = bytes_to_read_num_arg;
    }
    std::size_t min_size_required = start_byte_string_num + bytes_to_read_num;
    if(bytes_to_return.size() < min_size_required)
    {
      bytes_to_return.resize(min_size_required, '\0');
    }
    file.seekg(start_byte_file_num, std::ios::beg);
    file.read(&bytes_to_return[start_byte_string_num], bytes_to_read_num);
    file.close();
  }
  else
  {
    bytes_to_return.reserve(expected_file_size * 2);
    char buffer[read_buffer_size];
    while (file.read(buffer, sizeof(buffer)))
    {
      bytes_to_return.append(buffer, sizeof(buffer));
    }
    bytes_to_return.append(buffer, file.gcount());
//    return ret;
//    bytes_to_return =
//      (std::string((std::istreambuf_iterator<char>(file)),
//                    std::istreambuf_iterator<char>()));
  }

  return bytes_to_return;
}

std::string read_file(const std::string &path_string,
                      const bool binary,
                      const int start_byte_string_num,
                      const int start_byte_file_num,
                      const int bytes_to_read_num_arg,
                      const std::string &file_name_error)
{
  std::ios_base::openmode mode = std::ios_base::in;
  if(binary == TRACTOR_CONVERTER_BINARY)
  {
    mode |= std::ios_base::binary;
  }
  boost::filesystem::ifstream file(path_string, mode);
  if(!file)
  {
    throw exception::file_not_found(
      "Can't open " + file_name_error + " file \"" + path_string + "\".");
  }

  return read_file(file,
                   binary,
                   start_byte_string_num,
                   start_byte_file_num,
                   bytes_to_read_num_arg,
                   file_name_error);
}

std::string read_file(const boost::filesystem::path &path,
                      const bool binary,
                      const int start_byte_string_num,
                      const int start_byte_file_num,
                      const int bytes_to_read_num_arg,
                      const std::string &file_name_error)
{
  std::ios_base::openmode mode = std::ios_base::in;
  if(binary == TRACTOR_CONVERTER_BINARY)
  {
    mode |= std::ios_base::binary;
  }
  boost::filesystem::ifstream file(path, mode);
  if(!file)
  {
    throw exception::file_not_found(
      "Can't open " + file_name_error + " file \"" + path.string() + "\".");
  }

  return read_file(file,
                   binary,
                   start_byte_string_num,
                   start_byte_file_num,
                   bytes_to_read_num_arg,
                   file_name_error);
}






void write_to_file(boost::filesystem::ofstream &file,
                   const std::string &bytes_to_write,
                   const bool binary,
                   const int start_byte_string_num,
                   const int start_byte_file_num,
                   const int bytes_to_write_num_arg,
                   const std::string &file_name_error)
{
  if(binary == TRACTOR_CONVERTER_BINARY)
  {
    std::size_t bytes_to_write_num;
    if(bytes_to_write_num_arg == TRACTOR_CONVERTER_FILE_WRITE_ALL)
    {
      bytes_to_write_num = bytes_to_write.size() - start_byte_string_num;
    }
    else
    {
      bytes_to_write_num = bytes_to_write_num_arg;
    }
    file.seekp(start_byte_file_num, std::ios::beg);
    file.write(&bytes_to_write[start_byte_string_num], bytes_to_write_num);
    file.close();
  }
  else
  {
    file << bytes_to_write;
  }
}



void write_to_file(const std::string &path_string,
                   const std::string &bytes_to_write,
                   const bool overwrite,
                   const bool binary,                   
                   const int start_byte_string_num,
                   const int start_byte_file_num,
                   const int bytes_to_write_num_arg,
                   const std::string &file_name_error)
{
  std::ios_base::openmode mode = std::ios_base::out;
  if(overwrite == TRACTOR_CONVERTER_FILE_INSERT)
  {
     mode |= std::ios_base::in;
  }
  if(binary == TRACTOR_CONVERTER_BINARY)
  {
    mode |= std::ios_base::binary;
  }
  boost::filesystem::ofstream file(path_string, mode);
  if(!file)
  {
    throw exception::file_not_saved(
      "Can't save " + file_name_error + " file \"" + path_string + "\".");
  }

  write_to_file(file,
                bytes_to_write,
                binary,
                start_byte_string_num,
                start_byte_file_num,
                bytes_to_write_num_arg,
                file_name_error);
}

void write_to_file(const boost::filesystem::path &path,
                   const std::string &bytes_to_write,
                   const bool overwrite,
                   const bool binary,
                   const int start_byte_string_num,
                   const int start_byte_file_num,
                   const int bytes_to_write_num_arg,
                   const std::string &file_name_error)
{
  std::ios_base::openmode mode = std::ios_base::out;
  if(overwrite == TRACTOR_CONVERTER_FILE_INSERT)
  {
     mode |= std::ios_base::in;
  }
  if(binary == TRACTOR_CONVERTER_BINARY)
  {
    mode |= std::ios_base::binary;
  }
  boost::filesystem::ofstream file(path, mode);
  if (!file)
  {
    throw exception::file_not_saved(
      "Can't save " + file_name_error + " file \"" + path.string() + "\".");
  }

  write_to_file(file,
                bytes_to_write,
                binary,
                start_byte_string_num,
                start_byte_file_num,
                bytes_to_write_num_arg,
                file_name_error);
}



void save_file(const std::string &path_string,
               const std::string &bytes_to_write,
               bool binary,
               const std::string &file_name_error)
{
  std::ios_base::openmode mode = std::ios_base::out;
  if(binary == TRACTOR_CONVERTER_BINARY)
  {
    mode |= std::ios_base::binary;
  }

  boost::filesystem::ofstream file(path_string, mode);
  if(!file)
  {
    throw exception::file_not_saved(
      "Can't save " + file_name_error + " file \"" + path_string + "\".");
  }

  write_to_file(file,
                bytes_to_write,
                binary,
                0,
                0,
                TRACTOR_CONVERTER_FILE_WRITE_ALL,
                file_name_error);
}

void save_file(const boost::filesystem::path &path,
               const std::string &bytes_to_write,
               bool binary,
               const std::string &file_name_error)
{
  std::ios_base::openmode mode = std::ios_base::out;
  if(binary == TRACTOR_CONVERTER_BINARY)
  {
    mode |= std::ios_base::binary;
  }
  boost::filesystem::ofstream file(path, mode);
  if(!file)
  {
    throw exception::file_not_saved(
      "Can't save " + file_name_error + " file \"" + path.string() + "\".");
  }

  write_to_file(file,
                bytes_to_write,
                binary,
                0,
                0,
                TRACTOR_CONVERTER_FILE_WRITE_ALL,
                file_name_error);
}





boost::filesystem::path get_directory(const std::string &path_string,
                                      const std::string &dir_name_error)
{
  boost::filesystem::path dir =
    boost::filesystem::system_complete(path_string);

  if(!boost::filesystem::exists(dir))
  {
    throw exception::directory_not_found(
      dir_name_error + " directory " +
      "\"" + dir.string() + "\" does not exist.");
  }
  if(!boost::filesystem::is_directory(dir))
  {
    throw exception::directory_not_found(
      dir_name_error + " directory " +
      "\"" + dir.string() + "\" is not a directory.");
  }

  return dir;
}



} // namespace helpers
} // namespace tractor_converter
