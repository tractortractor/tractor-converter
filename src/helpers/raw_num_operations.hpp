#ifndef TRACTOR_CONVERTER_RAW_NUM_OPERATIONS_H
#define TRACTOR_CONVERTER_RAW_NUM_OPERATIONS_H

#include "defines.hpp"

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
std::vector<T> raw_bytes_to_vec_num(
  const std::string &bytes, std::size_t pos_to_read, std::size_t count_el)
{
  std::vector<T> vec(count_el);
  for(auto &&num : vec)
  {
    num = raw_bytes_to_num<T>(bytes, pos_to_read);
    pos_to_read += sizeof(T);
  }
  return vec;
}

template<typename T>
std::vector<std::vector<T>> raw_bytes_to_nest_vec_num(
  const std::string &bytes,
  std::size_t pos_to_read,
  const std::vector<std::size_t> &count_map)
{
  std::size_t count_map_size = count_map.size();
  std::vector<std::vector<T>> nest_vec(count_map_size);
  for(std::size_t cur_el = 0; cur_el < count_map_size; ++cur_el)
  {
    nest_vec[cur_el] =
      raw_bytes_to_vec_num<T>(bytes, pos_to_read, count_map[cur_el]);
    pos_to_read += sizeof(T) * count_map[cur_el];
  }
  return nest_vec;
}

template<typename T>
std::vector<std::vector<T>> raw_bytes_to_nest_vec_num(
  const std::string &bytes,
  std::size_t pos_to_read,
  std::size_t count_vec,
  std::size_t count_el)
{
  std::vector<std::vector<T>> nest_vec(count_vec);
  for(auto &&vec : nest_vec)
  {
    vec = raw_bytes_to_vec_num<T>(bytes, pos_to_read, count_el);
    pos_to_read += sizeof(T) * count_el;
  }
  return nest_vec;
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
