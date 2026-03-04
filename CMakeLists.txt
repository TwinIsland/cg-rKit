cmake_minimum_required(VERSION 3.16)

project(ball_minimal_gen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

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

# Each executable target must define itself in its own folder CMakeLists.txt.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/welcome_module/CMakeLists.txt")
  add_subdirectory(welcome_module)
endif()
