#ifndef TRACTOR_CONVERTER_CHECK_OPTION_H
#define TRACTOR_CONVERTER_CHECK_OPTION_H

#include "defines.hpp"

#include <boost/program_options.hpp>

#include <exception>
#include <stdexcept>

#include <string>
#include <vector>

namespace tractor_converter{
namespace helpers{

int check_option(const boost::program_options::variables_map &options,
                 const std::string &option_name,
                 const int throw_on_failure = TRACTOR_CONVERTER_THROW_ON_FAILURE);
int check_options(const boost::program_options::variables_map &options,
                  const std::vector<std::string> &option_names,
                  const int throw_on_failure = TRACTOR_CONVERTER_THROW_ON_FAILURE);

} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_CHECK_OPTION_H
