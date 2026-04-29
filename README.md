# cg-rKit

A lightweight C++ framework for fast computer graphics research prototypes.

## Build / Run

```sh
cc build.c -o build_tool && ./build_tool --run
```

Always-on dependency:

- nlohmann/json

Optional dependencies:

- Eigen3
- Knitro
- oneTBB
- libigl
- CGAL

## Developers

For the first setup on a new project, configure both option scopes:

```sh
./build_tool --config-all
```

The generated option files are split by ownership:

- `.project_opt` is project-level. It stores dependency flags that should stay consistent across machines.
- `.user_opt` is user-level. It stores machine-local settings.

On normal setup or after editing `.project_opt` or `.user_opt`, run `--config`.
It reloads both serialized option files and only prompts for the file that is
missing:

```sh
./build_tool --config
```

When you want to re-answer both project dependency and local machine prompts,
run:

```sh
./build_tool --config-all
```


### CLI:

```sh
usage: ./build_tool [options]

build:
  (no flags)       debug build; runs configure if no CMakePresets.json
  --release        release build

run:
  --run            run debug executable (builds first if needed)
  --run --release  run release executable (builds first if needed)

configure:
  --config         reload .project_opt and .user_opt; prompt only for missing files
  --config-all     reconfigure .project_opt and .user_opt from scratch
```
