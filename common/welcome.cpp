#include "common/welcome.hpp"

#include <iostream>

void _WELCOME_TO_RKIT(std::ostream& os) {
  os << "\n";
  os << "  ____  _  ___ _   \n";
  os << " |  _ \\| |/ (_) |_ \n";
  os << " | |_) | ' /| | __|\n";
  os << " |  _ <| . \\| | |_ \n";
  os << " |_| \\_\\_|\\_\\_|\\__|\n";
  os << "\n";
  os << "  rKit ready.\n";
  os << "  Layout: <module>.cpp + <module>/CMakeLists.txt + optional <module>/*.cpp\n";
  os << "  New external: add external/cmake/NNNN_<name>.cmake with FetchContent + register_external_target(...).\n";
  os << "  Then run: cmake -S . -B build && cmake --build build\n";
}

void _WELCOME_TO_RKIT() {
  _WELCOME_TO_RKIT(std::cout);
}
