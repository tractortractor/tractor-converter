#ifndef TRACTOR_CONVERTER_to_string_precision_H
#define TRACTOR_CONVERTER_to_string_precision_H



#include <cstdlib>
#include <string>
#include <limits>
#include <cmath>

namespace tractor_converter{
namespace helpers{

template<typename T>
void to_string_precision(T num_to_convert,
                         const std::string &format_string,
                         std::string &string_to_modify);

} // namespace helpers
} // namespace tractor_converter



#endif // TRACTOR_CONVERTER_to_string_precision_H
