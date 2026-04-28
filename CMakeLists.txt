cmake_minimum_required(VERSION 3.16)

project(ball_minimal_gen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/CMakeLists.txt")
  add_subdirectory(external)
endif()

# Aggregates all auto-registered external libraries.
add_library(project_external_deps INTERFACE)
if(PROJECT_EXTERNAL_TARGETS)
  target_link_libraries(project_external_deps INTERFACE ${PROJECT_EXTERNAL_TARGETS})
endif()

# Shared code layout:
# - headers: ./includes/common/*.hpp
# - sources: ./common/*.cpp
file(
  GLOB_RECURSE COMMON_SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp"
)

if(COMMON_SOURCES)
  add_library(project_common STATIC ${COMMON_SOURCES})
  target_include_directories(
    project_common
    PUBLIC
      "${CMAKE_CURRENT_SOURCE_DIR}"
      "${CMAKE_CURRENT_SOURCE_DIR}/includes"
  )
  if(TARGET project_external_deps)
    target_link_libraries(project_common PRIVATE project_external_deps)
  endif()
endif()

# Main module layout:
# - headers: ./includes/*.hpp
# - sources: ./src/*.cpp
# - optimizing: ./optimizing
file(
  GLOB_RECURSE SRC_SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(
  GLOB_RECURSE OPTIMIZING_SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/optimizing/*.cpp"
)

list(APPEND SRC_SOURCES ${OPTIMIZING_SOURCES})

if(SRC_SOURCES)
  add_library(project_src STATIC ${SRC_SOURCES})
  target_include_directories(
    project_src
    PUBLIC
      "${CMAKE_CURRENT_SOURCE_DIR}"
      "${CMAKE_CURRENT_SOURCE_DIR}/includes"
  )

  if(TARGET project_common)
    target_link_libraries(project_src PUBLIC project_common)
  endif()

  if(TARGET project_external_deps)
    target_link_libraries(project_src PUBLIC project_external_deps)
  endif()
endif()

# Executable entry points under ./main
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main/CMakeLists.txt")
  add_subdirectory(main)
endif()

# Test entry points under ./tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
  add_subdirectory(tests)
endif()
