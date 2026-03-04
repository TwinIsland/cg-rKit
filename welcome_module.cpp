#include <iostream>

#include "common.hpp"
#include "welcome_module.hpp"

int main() {
  std::cout << common_greeting() << '\n';
  std::cout << welcome_module_greeting() << '\n';
  _WELCOME_TO_RKIT();
  return 0;
}
