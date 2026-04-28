#pragma once

#include <fstream>
#include <ostream>
#include <string>
#include <vector>

namespace csv {
std::ofstream open_writer(const std::string &path,
                          const std::vector<std::string> &header);

void write_row(std::ostream &stream, const std::vector<std::string> &fields);

void close_writer(std::ofstream &stream);
} // namespace csv
