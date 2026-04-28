# Common Module Quick Guide

This folder contains shared utilities used across modules:
- `Logger` for console/file logs
- `State` for simple typed key-value persistence (`json`)

## Logger

Header: `includes/common/logger.hpp`

```cpp
#include "common/logger.hpp"

int main() {
  Logger logger("my_module");

  logger.info("started");
  logger.warning("using default configuration");

  // Preferred in code paths where file/line/function context is useful:
  LOG_INFO(logger, "loading mesh");
  LOG_DANGER(logger, "failed to open file");
}
```

## State

Header: `includes/common/state.hpp`

```cpp
#include "common/state.hpp"

int main() {
  State s;
  s.set("iter", 12);
  s.set("loss", 0.031);
  s.set("name", std::string("run_a"));
  s.set("values", std::vector<double>{1.0, 2.0, 3.0});
  s.save("data/run_a.state.json");

  State loaded = State::load("data/run_a.state.json");
  auto iter = loaded.get<int>("iter");
  auto loss = loaded.get<double>("loss");
}
```

Supported `State` value types:
- scalar: `int`-like, `double`/`float`, `bool`, `std::string`, `const char*`
- lists: `std::vector<int-like>`, `std::vector<double/float>`, `std::vector<bool>`, `std::vector<std::string>`
- `enum` (stored as integer)

Not supported directly:
- custom structs/classes, nested maps/objects, pointers

For custom structs, flatten fields into multiple keys.
