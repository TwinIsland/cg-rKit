cmake_minimum_required(VERSION 3.16)

set(target_name welcome_module)

set(main_file "${PROJECT_SOURCE_DIR}/welcome_module.cpp")
file(
  GLOB_RECURSE target_sources
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

if(EXISTS "${main_file}")
  list(PREPEND target_sources "${main_file}")
endif()

if(target_sources STREQUAL "")
  message(FATAL_ERROR "No source files found for target '${target_name}'.")
endif()

add_executable(${target_name} ${target_sources})

target_include_directories(
  ${target_name}
  PRIVATE
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/includes"
)

if(TARGET project_common)
  target_link_libraries(${target_name} PRIVATE project_common)
endif()

if(TARGET project_external_deps)
  target_link_libraries(${target_name} PRIVATE project_external_deps)
endif()
