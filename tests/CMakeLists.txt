cmake_minimum_required(VERSION 3.16)

if(NOT TARGET project_src)
  message(FATAL_ERROR "project_src target is required before adding tests.")
endif()

file(
  GLOB TEST_SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

set(TEST_EXECUTABLES "")

foreach(test_src IN LISTS TEST_SOURCES)
  get_filename_component(test_name "${test_src}" NAME_WE)
  set(test_target "test_${test_name}")

  add_executable(${test_target} "${test_src}")
  target_link_libraries(${test_target} PRIVATE project_src)

  target_include_directories(
    ${test_target}
    PRIVATE
      "${PROJECT_SOURCE_DIR}"
      "${PROJECT_SOURCE_DIR}/includes"
  )

  list(APPEND TEST_EXECUTABLES "${test_target}")
endforeach()