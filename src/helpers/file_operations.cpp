#include "file_operations.hpp"



namespace tractor_converter{
namespace helpers{



std::string read_file(boost::filesystem::ifstream &file,
                      const bitflag<file_flag> flags,
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

  if(flags & file_flag::binary)
  {
    if(flags & file_flag::read_all)
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

std::string read_file(const boost::filesystem::path &path,
                      const bitflag<file_flag> flags,
                      const int start_byte_string_num,
                      const int start_byte_file_num,
                      const int bytes_to_read_num_arg,
                      const std::string &file_name_error)
{
  std::ios_base::openmode mode = std::ios_base::in;
  if(flags & file_flag::binary)
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
                   flags,
                   start_byte_string_num,
                   start_byte_file_num,
                   bytes_to_read_num_arg,
                   file_name_error);
}

std::string read_file(const std::string &path_string,
                      const bitflag<file_flag> flags,
                      const int start_byte_string_num,
                      const int start_byte_file_num,
                      const int bytes_to_read_num_arg,
                      const std::string &file_name_error)
{
  return read_file(boost::filesystem::path(path_string),
                   flags,
                   start_byte_string_num,
                   start_byte_file_num,
                   bytes_to_read_num_arg,
                   file_name_error);
}





void write_to_file(boost::filesystem::ofstream &file,
                   const std::string &bytes_to_write,
                   const bitflag<file_flag> flags,
                   const int start_byte_string_num,
                   const int start_byte_file_num,
                   const int bytes_to_write_num_arg,
                   const std::string &file_name_error)
{
  if(flags & file_flag::binary)
  {
    std::size_t bytes_to_write_num;
    if(flags & file_flag::write_all)
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



void write_to_file(const boost::filesystem::path &path,
                   const std::string &bytes_to_write,
                   const bitflag<file_flag> flags,
                   const int start_byte_string_num,
                   const int start_byte_file_num,
                   const int bytes_to_write_num_arg,
                   const std::string &file_name_error)
{
  std::ios_base::openmode mode = std::ios_base::out;
  if(!(flags & file_flag::overwrite))
  {
     mode |= std::ios_base::in;
  }
  if(flags & file_flag::binary)
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
                flags,
                start_byte_string_num,
                start_byte_file_num,
                bytes_to_write_num_arg,
                file_name_error);
}

void write_to_file(const std::string &path_string,
                   const std::string &bytes_to_write,
                   const bitflag<file_flag> flags,
                   const int start_byte_string_num,
                   const int start_byte_file_num,
                   const int bytes_to_write_num_arg,
                   const std::string &file_name_error)
{
  write_to_file(boost::filesystem::path(path_string),
                bytes_to_write,
                flags,
                start_byte_string_num,
                start_byte_file_num,
                bytes_to_write_num_arg,
                file_name_error);
}



void save_file(const boost::filesystem::path &path,
               const std::string &bytes_to_write,
               const bitflag<file_flag> flags,
               const std::string &file_name_error)
{
  std::ios_base::openmode mode = std::ios_base::out;
  if(flags & file_flag::binary)
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
                flags | file_flag::write_all,
                0,
                0,
                write_all_dummy_size,
                file_name_error);
}

void save_file(const std::string &path_string,
               const std::string &bytes_to_write,
               const bitflag<file_flag> flags,
               const std::string &file_name_error)
{
  save_file(boost::filesystem::path(path_string),
            bytes_to_write,
            flags,
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





boost::filesystem::path filepath_case_insensitive_part_get(
  const boost::filesystem::path &case_sensitive_part,
  const boost::filesystem::path &case_insensitive_part)
{
  std::string case_insensitive_lowercase_str =
    boost::algorithm::to_lower_copy(case_insensitive_part.string());

  for(const auto &entry :
      boost::filesystem::recursive_directory_iterator(case_sensitive_part))
  {
    boost::filesystem::path rel_path_to_cmp =
      entry.path().lexically_relative(case_sensitive_part);
    std::string rel_path_to_cmp_lowercase_str =
      boost::algorithm::to_lower_copy(rel_path_to_cmp.string());
    if(rel_path_to_cmp_lowercase_str == case_insensitive_lowercase_str)
    {
      return entry.path();
    }
  }
  return boost::filesystem::path();
}



} // namespace helpers
} // namespace tractor_converter
