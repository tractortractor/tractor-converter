#include "hex.hpp"



namespace tractor_converter{
namespace helpers{



template<>
std::string int_to_hex_string<char>(const char to_hex)
{
  std::stringstream string_stream_intermediate;
  string_stream_intermediate << std::hex <<
    static_cast<std::uint16_t>(static_cast<unsigned char>(to_hex));
  return string_stream_intermediate.str();
}

template<>
std::string int_to_hex_string<unsigned char>(const unsigned char to_hex)
{
  std::stringstream string_stream_intermediate;
  string_stream_intermediate << std::hex << static_cast<std::uint16_t>(to_hex);
  return string_stream_intermediate.str();
}



} // namespace helpers
} // namespace tractor_converter
