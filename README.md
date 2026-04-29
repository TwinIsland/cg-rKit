# cg-rKit

A lightweight C++ framework for fast computer graphics research prototypes.

## Build

The repository includes a small metabuild helper that generates CMake presets
for the optional external libraries.

```sh
cc build.c -o build_tool && ./build_tool
```

Useful commands:

```sh
./build_tool -h         # more detailed help
./build_tool --release  # build with release flag
./build_tool --run      # run executable
./build_tool --config   # reconfig user-level configs
```

Always-on dependency:

- nlohmann/json

Optional dependencies:

- Eigen3
- Knitro
- oneTBB
- libigl
- CGAL
