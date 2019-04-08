#include "check_option.hpp"



namespace tractor_converter{
namespace helpers{



int check_option(const boost::program_options::variables_map &options,
                 const std::string &option_name,
                 const int throw_on_failure)
{
  if(!options.count(option_name))
  {
    if(throw_on_failure == TRACTOR_CONVERTER_THROW_ON_FAILURE)
    {
      throw std::runtime_error("\"" + option_name + "\" option not specified");
    }
    return 0;
  }
  return 1;
}

int check_options(const boost::program_options::variables_map &options,
                  const std::vector<std::string> &option_names,
                  const int throw_on_failure)
{
  for(const auto &option_name : option_names)
  {
    if(!check_option(options, option_name, throw_on_failure))
    {
      return 0;
    }
  }
  return 1;
}



} // namespace helpers
} // namespace tractor_converter
