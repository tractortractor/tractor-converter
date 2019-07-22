#include "vangers_cfg_operations.hpp"



namespace tractor_converter{
namespace helpers{



sicher_cfg_reader::sicher_cfg_reader(
  std::string &&str_arg,
  const std::string &input_file_path_str_arg,
  const std::string &input_file_name_error_arg)
: m_str(std::move(str_arg)),
  input_file_path_str(input_file_path_str_arg),
  input_file_name_error(input_file_name_error_arg),
  pos(&m_str[0]),
  end_pos(&m_str[m_str.size()])
{
}



template<> int sicher_cfg_reader::get_next_value_helper_convert_str<int>()
{
  return std::strtol(pos, &pos, 0);
}

template<> double
  sicher_cfg_reader::get_next_value_helper_convert_str<double>()
{
  return std::strtod(pos, &pos);
}

template<> std::string
  sicher_cfg_reader::get_next_value_helper_convert_str<std::string>()
{
  while(std::iswspace(*pos))
  {
    if(pos > end_pos)
    {
      throw std::runtime_error(
        input_file_name_error + " file " +
        input_file_path_str +
        " unexpected end of the file while attempting to read string value.");
    }
    ++pos;
  }

  char *value_beg_pos = pos;
  if(*pos == '"')
  {
    ++value_beg_pos;
    for(; pos < end_pos; ++pos)
    {
      if(*pos == '"' || (std::iswspace(*pos) && *pos != ' ' && *pos != '\t'))
      {
        break;
      }
    }
  }
  else
  {
    for(; pos < end_pos; ++pos)
    {
      if(std::iswspace(*pos))
      {
        break;
      }
    }
  }

  return std::string(value_beg_pos,
                     static_cast<std::size_t>(pos - value_beg_pos));
}



void sicher_cfg_reader::move_pos_to_value(const std::string &value_name)
{
  pos = std::strstr(pos, value_name.c_str()) + value_name.size();
}



template<typename T>
T sicher_cfg_reader::get_next_value(const std::string &value_name)
{
  move_pos_to_value(value_name);
  return get_next_value_helper_convert_str<T>();
}

template int sicher_cfg_reader::get_next_value<int>(
  const std::string &value_name);
template double sicher_cfg_reader::get_next_value<double>(
  const std::string &value_name);
template std::string sicher_cfg_reader::get_next_value<std::string>(
  const std::string &value_name);



const std::string &sicher_cfg_reader::str()
{
  return m_str;
}





sicher_cfg_writer::sicher_cfg_writer(
  std::string &&str_arg,
  const std::string &input_file_path_str_arg,
  const std::string &input_file_name_error_arg,
  std::size_t size_increase)
: sicher_cfg_reader(std::move(str_arg),
                    input_file_path_str_arg,
                    input_file_name_error_arg)
{
  m_out_str.reserve(m_str.size() + size_increase);
  copied_pos = &m_str[0];
}



void sicher_cfg_writer::write_until_pos()
{
  m_out_str.append(
    m_str,
    static_cast<std::size_t>(copied_pos - &m_str[0]),
    static_cast<std::size_t>(pos - copied_pos));
  copied_pos = pos;
}



void sicher_cfg_writer::write_until_end()
{
  m_out_str.append(
    m_str,
    static_cast<std::size_t>(copied_pos - &m_str[0]),
    static_cast<std::size_t>(const_cast<char*>(end_pos) - copied_pos));
  copied_pos = const_cast<char*>(end_pos);
}



template<> void
  sicher_cfg_writer::overwrite_next_value_helper_append_value<int>(
    const int &value,
    const std::string &format)
{
  to_string_precision<int>(value, format, m_out_str);
}



template<> void
  sicher_cfg_writer::overwrite_next_value_helper_append_value<double>(
    const double &value,
    const std::string &format)
{
  to_string_precision<double>(value, format, m_out_str);
}



template<> void
  sicher_cfg_writer::overwrite_next_value_helper_move_pos_after_value<int>()
{
  std::strtol(pos, &pos, 0);
}



template<> void
  sicher_cfg_writer::overwrite_next_value_helper_move_pos_after_value<double>()
{
  std::strtod(pos, &pos);
}



template<typename T>
void sicher_cfg_writer::overwrite_next_value(const std::string &value_name,
                                             T value,
                                             const std::string &format)
{
  move_pos_to_value(value_name);
  while(std::iswspace(*pos))
  {
    if(pos > end_pos)
    {
      throw std::runtime_error(
        input_file_name_error +
        " file " + input_file_path_str +
        " unexpected end of the file while attempting to overwrite value.");
    }
    ++pos;
  }
  write_until_pos();

  overwrite_next_value_helper_append_value<T>(value, format);

  overwrite_next_value_helper_move_pos_after_value<T>();
  copied_pos = pos;
}



template void sicher_cfg_writer::overwrite_next_value<int>(
  const std::string &value_name, int value, const std::string &format);
template void sicher_cfg_writer::overwrite_next_value<double>(
  const std::string &value_name, double value, const std::string &format);



const std::string &sicher_cfg_writer::out_str()
{
  return m_out_str;
}



} // namespace helpers
} // namespace tractor_converter
