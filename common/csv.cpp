#include "common/csv.hpp"

#include <filesystem>
#include <stdexcept>

namespace {
std::string escape_csv_field(const std::string &field) {
  bool needs_quotes = false;
  std::string escaped;
  escaped.reserve(field.size());

  for (char ch : field) {
    if (ch == '"' || ch == ',' || ch == '\n') {
      needs_quotes = true;
    }
    if (ch == '"') {
      escaped += "\"\"";
    } else {
      escaped.push_back(ch);
    }
  }

  if (!needs_quotes) {
    return escaped;
  }

  return "\"" + escaped + "\"";
}
} // namespace

namespace csv {
std::ofstream open_writer(const std::string &path,
                          const std::vector<std::string> &header) {
  const std::filesystem::path out_path(path);
  if (out_path.has_parent_path()) {
    std::filesystem::create_directories(out_path.parent_path());
  }

  std::ofstream stream(path);
  if (!stream.is_open()) {
    throw std::runtime_error("Failed to open CSV file for write: " + path);
  }

  write_row(stream, header);
  return stream;
}

void write_row(std::ostream &stream, const std::vector<std::string> &fields) {
  for (std::size_t i = 0; i < fields.size(); ++i) {
    if (i > 0) {
      stream << ',';
    }
    stream << escape_csv_field(fields[i]);
  }
  stream << '\n';
  stream.flush();
}

void close_writer(std::ofstream &stream) {
  if (stream.is_open()) {
    stream.flush();
    stream.close();
  }
}
} // namespace csv
