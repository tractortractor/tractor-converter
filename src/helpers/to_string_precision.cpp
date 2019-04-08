#include "to_string_precision.hpp"



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

template void to_string_precision<float>(float num_to_convert,
                                         const std::string &format_string,
                                         std::string &string_to_modify);
template void to_string_precision<double>(double num_to_convert,
                                          const std::string &format_string,
                                          std::string &string_to_modify);
template void to_string_precision<int>(int num_to_convert,
                                       const std::string &format_string,
                                       std::string &string_to_modify);

} // namespace helpers
} // namespace tractor_converter
