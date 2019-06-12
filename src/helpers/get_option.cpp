#include "get_option.hpp"



namespace tractor_converter{
namespace helpers{



std::vector<std::string> get_vec_str_option(
  const boost::program_options::variables_map &options,
  const std::string &option_name,
  const bitflag<error_handling> error_flags)
{
  std::vector<std::string> option;
  if(helpers::check_option(options,
                           option_name,
                           error_flags))
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



} // namespace helpers
} // namespace tractor_converter
