#pragma once

#include <string>

#include "CONFIG.H"            // IWYU pragma: export
#include "common/logger.hpp"   // IWYU pragma: export
#include "common/state.hpp"    // IWYU pragma: export
#include "common/welcome.hpp"  // IWYU pragma: export

static std::string common_greeting() {
  return "Hello from common";
}
