#ifndef TRACTOR_CONVERTER_HEX_H
#define TRACTOR_CONVERTER_HEX_H



#include <sstream>
#include <string>

namespace tractor_converter{
namespace helpers{

std::string char_arr_to_hex_string(const char *char_arr,
                                   unsigned int char_arr_size);

//std::string int_to_hex_string(int to_hex);
template<typename T>
std::string int_to_hex_string(const T to_hex);

} // namespace helpers
} // namespace tractor_converter



#endif // TRACTOR_CONVERTER_HEX_H
