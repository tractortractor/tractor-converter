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
                         std::string &string_to_modify)
{
  std::size_t cur_string_size = string_to_modify.size();
  std::size_t num_to_append =
    std::snprintf(nullptr, 0, format_string.c_str(), num_to_convert);
  string_to_modify.append(num_to_append, '\0');

  std::sprintf(&string_to_modify[cur_string_size],
               format_string.c_str(),
               num_to_convert);
}

} // namespace helpers
} // namespace tractor_converter



#endif // TRACTOR_CONVERTER_to_string_precision_H
