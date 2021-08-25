#ifndef READLINE_HPP
#define READLINE_HPP

#include <string>

namespace readline
{

bool read(std::string& input, const std::string& prompt = "punky >> ",
          const std::string& history_path = ".punky_history");

};  // namespace readline

#endif  // READLINE_HPP
