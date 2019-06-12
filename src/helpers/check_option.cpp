#include "check_option.hpp"



namespace tractor_converter{
namespace helpers{



int check_option(const boost::program_options::variables_map &options,
                 const std::string &option_name,
                 const bitflag<error_handling> error_flags)
{
  if(!options.count(option_name))
  {
    if(error_flags & error_handling::throw_exception)
    {
      throw std::runtime_error("\"" + option_name + "\" option not specified");
    }
    return 0;
  }
  return 1;
}

int check_options(const boost::program_options::variables_map &options,
                  const std::vector<std::string> &option_names,
                  const bitflag<error_handling> error_flags)
{
  for(const auto &option_name : option_names)
  {
    if(!check_option(options, option_name, error_flags))
    {
      return 0;
    }
  }
  return 1;
}



} // namespace helpers
} // namespace tractor_converter
