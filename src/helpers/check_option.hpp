#ifndef TRACTOR_CONVERTER_CHECK_OPTION_H
#define TRACTOR_CONVERTER_CHECK_OPTION_H

#include "defines.hpp"
#include "bitflag.hpp"

#include <boost/program_options.hpp>

#include <exception>
#include <stdexcept>

#include <string>
#include <vector>

namespace tractor_converter{
namespace helpers{

int check_option(const boost::program_options::variables_map &options,
                 const std::string &option_name,
                 const bitflag<error_handling> error_flags =
                   error_handling::throw_exception);
int check_options(const boost::program_options::variables_map &options,
                  const std::vector<std::string> &option_names,
                  const bitflag<error_handling> error_flags =
                    error_handling::throw_exception);

} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_CHECK_OPTION_H
