cmake_minimum_required(VERSION 3.16)

if(NOT TARGET project_src)
  message(FATAL_ERROR "project_src target is required before adding main executables.")
endif()

file(
  GLOB MAIN_SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

set(MAIN_EXECUTABLES "")

foreach(main_src IN LISTS MAIN_SOURCES)
  get_filename_component(main_name "${main_src}" NAME_WE)
  set(main_target "${main_name}")

  add_executable(${main_target} "${main_src}")
  target_link_libraries(${main_target} PRIVATE project_src)

  target_include_directories(
    ${main_target}
    PRIVATE
      "${PROJECT_SOURCE_DIR}"
      "${PROJECT_SOURCE_DIR}/includes"
  )

  list(APPEND MAIN_EXECUTABLES "${main_target}")
endforeach()
