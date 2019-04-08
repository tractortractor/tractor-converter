#ifndef TRACTOR_CONVERTER_RAW_NUM_OPERATIONS_H
#define TRACTOR_CONVERTER_RAW_NUM_OPERATIONS_H

#include "defines.hpp"

//#include <boost/filesystem.hpp>
//#include <boost/filesystem/fstream.hpp>

#include <exception>
#include <stdexcept>

#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

namespace tractor_converter{
namespace helpers{



template<typename T>
T raw_bytes_to_num(const std::string &bytes, std::size_t pos_to_read)
{
  T num;
  std::memcpy(&num, &bytes[pos_to_read], sizeof(T));
  return num;
}



template<typename T>
void num_to_raw_bytes(const T num,
                      std::string &bytes,
                      std::size_t pos_to_write)
{
  std::memcpy(&bytes[pos_to_write], &num, sizeof(T));
}

template<typename T>
void vec_num_to_raw_bytes(
  const std::vector<T> &vec, std::string &bytes, std::size_t pos_to_write)
{
  for(const auto num : vec)
  {
    num_to_raw_bytes<T>(num, bytes, pos_to_write);
    pos_to_write += sizeof(T);
  }
}

template<typename T>
void nest_vec_num_to_raw_bytes(
  const std::vector<std::vector<T>> &nest_vec,
  std::string &bytes,
  std::size_t pos_to_write)
{
  for(const auto &vec : nest_vec)
  {
    vec_num_to_raw_bytes<T>(vec, bytes, pos_to_write);
    pos_to_write += sizeof(T) * vec.size();
  }
}



} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_RAW_NUM_OPERATIONS_H
