#include "parse_mtl_body_offs.hpp"



namespace tractor_converter{
namespace helpers{



c3d::color::offset_map parse_mtl_body_offs(
  const std::vector<std::string> &body_offsets_str)
{
  c3d::color::offset_map body_offsets;
  for(const auto &offset_str : body_offsets_str)
  {
    std::size_t cur_pos = 0;
    while(true)
    {
      int color_offset;
      int color_shift;
      try
      {
        std::size_t chars_processed;
        if(cur_pos > offset_str.size())
        {
          break;
        }
        color_offset =
          std::stoi(offset_str.substr(cur_pos), &chars_processed);
        cur_pos += chars_processed + 1;
        if(cur_pos > offset_str.size())
        {
          break;
        }
        color_shift =
          std::stoi(offset_str.substr(cur_pos), &chars_processed);
        cur_pos += chars_processed + 1;
      }
      // If offset_str at position cur_pos does not contain
      // valid color_offset or shift_offset number.
      catch(std::invalid_argument &)
      {
        break;
      }
      body_offsets["body_offset_" + std::to_string(color_offset) +
                   "_shift_" + std::to_string(color_shift)] =
        c3d::color::offset_pair(color_offset, color_shift);
    }
  }
  return body_offsets;
}



} // namespace helpers
} // namespace tractor_converter
