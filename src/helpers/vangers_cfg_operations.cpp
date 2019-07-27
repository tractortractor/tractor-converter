#include "vangers_cfg_operations.hpp"



namespace tractor_converter{
namespace helpers{



exception::raw_uncompress_error::raw_uncompress_error(
  const char *err_msg_arg,
  int zlib_err_code_arg)
: m_zlib_err_code(zlib_err_code_arg)
{
  switch(zlib_err_code())
  {
    case Z_VERSION_ERROR:
      m_zlib_string += "zlib version mismatch.";
      break;
    case Z_STREAM_ERROR:
      m_zlib_string += "Internal stream error.";
      break;
    case Z_MEM_ERROR:
      m_zlib_string += "Not enough memory.";
      break;
    case Z_BUF_ERROR:
      m_zlib_string += "Not enough room in the output buffer.";
      break;
    case Z_DATA_ERROR:
      m_zlib_string += "Error data.";
      break;
  }
  if(err_msg_arg)
  {
    m_zlib_string += "\n" + std::string(err_msg_arg);
  }
}

const char* exception::raw_uncompress_error::what() const noexcept
{
  return m_zlib_string.c_str();
}

int exception::raw_uncompress_error::zlib_err_code() const noexcept
{
  return m_zlib_err_code;
}



std::string raw_uncompress(std::size_t decompressed_size,
                           std::string &compressed_str)
{
  std::string decompressed_str(decompressed_size, '\0');
  std::size_t compressed_size = compressed_str.size();

  z_stream stream;
  int err;
  const uInt max = static_cast<uInt>(-1);
  uLong len, left;

  len = compressed_size;
  if(decompressed_size)
  {
    left = decompressed_size;
  }
  else
  {
    left = 1;
    decompressed_str = std::string(1, '\0');
  }

  stream.next_in = reinterpret_cast<Bytef*>(&compressed_str[0]);
  stream.avail_in = 0;
  stream.zalloc = static_cast<alloc_func>(Z_NULL);
  stream.zfree = static_cast<free_func>(Z_NULL);
  stream.opaque = static_cast<voidpf>(Z_NULL);

  // -MAX_WBITS - because of negative value,
  // inflate() treats input as raw DEFLATE compression.
  err = inflateInit2(&stream, -MAX_WBITS);
  if (err != Z_OK)
  {
    throw exception::raw_uncompress_error(stream.msg, err);
  }

  stream.next_out = reinterpret_cast<Bytef*>(&decompressed_str[0]);
  stream.avail_out = 0;

  do
  {
    if(stream.avail_out == 0)
    {
      stream.avail_out =
        left > static_cast<uLong>(max) ? max : static_cast<uInt>(left);
      left -= stream.avail_out;
    }
    if(stream.avail_in == 0)
    {
      stream.avail_in =
        len > static_cast<uLong>(max) ? max : static_cast<uInt>(len);
      len -= stream.avail_in;
    }
    err = inflate(&stream, Z_NO_FLUSH);
  }
  while(err == Z_OK);



  if(stream.total_out && err == Z_BUF_ERROR)
  {
    left = 1;
  }

  inflateEnd(&stream);

  err = err == Z_STREAM_END ? Z_OK :
        err == Z_NEED_DICT ? Z_DATA_ERROR  :
        err == Z_BUF_ERROR && left + stream.avail_out ? Z_DATA_ERROR :
        err;

  if(err != Z_OK)
  {
    throw exception::raw_uncompress_error(stream.msg, err);
  }

  decompressed_str.resize(stream.total_out);
  return decompressed_str;
}



unsigned int xzip_crypt::crt(unsigned int &val)
{
  val ^= val >> 3;
  val ^= val << 28;
  val &= 0x7FFFFFFF;

  return val;
}



sicher_cfg_reader::sicher_cfg_reader(
  std::string &&str_arg,
  const std::string &input_file_path_str_arg,
  const std::string &input_file_name_error_arg)
: m_str(std::move(str_arg)),
  pos(&m_str[0]),
  end_pos(&m_str[m_str.size()]),
  input_file_path_str(input_file_path_str_arg),
  input_file_name_error(input_file_name_error_arg)
{
  // If first byte = 0 then file is compressed and encrypted.
  if(!*pos)
  {
    decrypt();
    decompress();
  }
}



void sicher_cfg_reader::decrypt()
{
  std::uint32_t key =
    raw_bytes_to_num<std::uint32_t>(m_str, xzip_crypt::key_pos);

  key *= xzip_crypt::key::multiplier;
  key |= xzip_crypt::key::bin_or;

  m_str.erase(0, xzip_crypt::enc_beg_pos);
  pos = &m_str[0];

  for(char& cur_char : m_str)
  {
    cur_char ^= xzip_crypt::crt(key);
  }
}

void sicher_cfg_reader::decompress()
{
  std::int16_t label =
    raw_bytes_to_num<std::int16_t>(m_str, xzip_decompress::label_pos);
  std::size_t decompressed_size =
    raw_bytes_to_num<std::uint32_t>(m_str, xzip_decompress::decomp_size_pos);
  decompressed_size += xzip_decompress::add_decomp_size;

  m_str.erase(0, xzip_decompress::comp_beg_pos);
  pos = &m_str[0];

  // If label = 0 then file is not compressed.
  if(label)
  {
    try
    {
      m_str = raw_uncompress(decompressed_size, m_str);
    }
    catch(exception::raw_uncompress_error &e)
    {
      throw std::runtime_error(
        input_file_name_error + " file " +
        input_file_path_str + " failed to decompress.\n" +
        e.what());
    }
    pos = &m_str[0];
    end_pos = &m_str[m_str.size()];
  }
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
