# cg-rKit

A lightweight C++ framework for fast computer graphics research prototypes.

## Build / Run

```sh
cc build.c -o build_tool && ./build_tool --run --release
```

Always-on dependency:

- nlohmann/json

Optional dependencies:

- Eigen3
- Knitro
- oneTBB
- libigl
- CGAL

## For Developer

For the first setup on a new project, configure both option scopes:

```sh
./build_tool --config-all
```

The generated option files are split by ownership:

- `.project_opt` is project-level. It stores dependency flags that should stay consistent across machines.
- `.user_opt` is user-level. It stores machine-local settings.

On normal setup or after editing `.project_opt` or `.user_opt`, run `--config`.
It reloads both serialized option files and only prompts for the file that is
missing.

```sh
./build_tool --config
```

**Add new external library:**

All external lirabry should be managed in the same manner as those in `external` folder.  First add a `cmake` to fetch the new library and make it available. Then, update `external/dependencies.txt` if needed. The format
is intentionally simple:

```text
USE_SOME_LIB: USE_EIGEN, USE_TBB
```

When presets are regenerated, `build_tool` checks `.project_opt` against this
file and asks whether to auto-enable missing required options.

Finally, update the meta build system `build.c` to register the new external library.

### CLI:

```sh
usage: ./build_tool [options]

build:
  (no flags)       reload saved options, then debug build
  --release        reload saved options, then release build

run:
  --run            run debug executable (builds first if needed)
  --run --release  run release executable (builds first if needed)

configure:
  --config         reload .project_opt and .user_opt; prompt only for missing files
  --config-all     reconfigure .project_opt and .user_opt from scratch
```
