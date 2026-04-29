set(NLOHMANN_JSON_ROOT "${PROJECT_SOURCE_DIR}/third-party/nlohmann_json")
set(NLOHMANN_JSON_HEADER "${NLOHMANN_JSON_ROOT}/json.hpp")

if(NOT EXISTS "${NLOHMANN_JSON_HEADER}")
  message(FATAL_ERROR "Cached nlohmann/json header not found: ${NLOHMANN_JSON_HEADER}")
endif()

add_library(project_nlohmann_json INTERFACE)
target_include_directories(project_nlohmann_json INTERFACE "${NLOHMANN_JSON_ROOT}")

add_library(nlohmann_json::nlohmann_json ALIAS project_nlohmann_json)

register_external_target(nlohmann_json::nlohmann_json)
