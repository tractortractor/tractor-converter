#include "check_pal_color_used.hpp"

namespace tractor_converter{
namespace helpers{

bool check_pal_color_used(const std::size_t pos, const std::string &bytes)
{
  for(std::size_t current_color_byte = 0;
      current_color_byte != tga_default_color_size;
      ++current_color_byte)
  {
    if(bytes[pos + current_color_byte])
    {
      return true;
    }
  }
  return false;
}

} // namespace helpers
} // namespace tractor_converter
