#ifndef TRACTOR_CONVERTER_HEX_H
#define TRACTOR_CONVERTER_HEX_H

#include <cstdint>
#include <sstream>
#include <string>



namespace tractor_converter{
namespace helpers{



template<typename T>
std::string int_to_hex_string(const T to_hex)
{
  std::stringstream string_stream_intermediate;
  string_stream_intermediate << std::hex << to_hex;
  return string_stream_intermediate.str();
}

template<>
std::string int_to_hex_string<char>(const char to_hex);

template<>
std::string int_to_hex_string<unsigned char>(const unsigned char to_hex);



} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_HEX_H
