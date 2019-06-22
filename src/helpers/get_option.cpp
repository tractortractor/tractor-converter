#include "get_option.hpp"



namespace tractor_converter{
namespace helpers{



std::vector<std::string> get_vec_str_option(
  const boost::program_options::variables_map &options,
  const std::string &option_name,
  const bitflag<error_handling> error_flags)
{
  std::vector<std::string> option;
  if(helpers::check_option(options, option_name, error_flags))
  {
    try
    {
      option = options[option_name].as<std::vector<std::string>>();
    }
    catch(boost::bad_any_cast &)
    {
      std::cout << "Failed to retrieve \"" << option_name << "\" " <<
        "as std::vector<std::string>." << '\n';
      throw;
    }
  }
  return option;
}



double get_angle_option(
  const boost::program_options::variables_map &options,
  const std::string &option_name,
  const bitflag<error_handling> error_flags)
{
  double angle = 0.0;
  std::size_t num_char_size;
  if(helpers::check_option(options, option_name, error_flags))
  {
    std::string angle_str = options[option_name].as<std::string>();
    try
    {
      angle = std::stod(angle_str, &num_char_size);
    }
    catch(std::invalid_argument &)
    {
      std::cout << "Failed to retrieve \"" << option_name << "\" " <<
        "as angle." << '\n';
      throw;
    }
    catch(std::out_of_range &)
    {
      std::cout << "Failed to retrieve \"" << option_name << "\" " <<
        "as angle." << '\n';
      throw;
    }
    if(num_char_size < angle_str.size() &&
       (angle_str.c_str()[num_char_size] == 'd' ||
        angle_str.c_str()[num_char_size] == 'D'))
    {
      angle = volInt::degrees_to_radians(angle);
    }
  }
  return angle;
}



} // namespace helpers
} // namespace tractor_converter
