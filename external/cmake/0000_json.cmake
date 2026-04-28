include(FetchContent)

set(JSON_BuildTests OFF CACHE BOOL "Build nlohmann/json tests" FORCE)
set(JSON_Install OFF CACHE BOOL "Install nlohmann/json targets" FORCE)

fetchcontent_declare(
  external_json
  GIT_REPOSITORY "https://github.com/nlohmann/json.git"
  GIT_TAG "v3.12.0"
  GIT_SHALLOW TRUE
)

fetchcontent_makeavailable(external_json)

if(TARGET nlohmann_json::nlohmann_json)
  register_external_target(nlohmann_json::nlohmann_json)
else()
  message(FATAL_ERROR "nlohmann/json fetch succeeded but no CMake target was exposed.")
endif()
