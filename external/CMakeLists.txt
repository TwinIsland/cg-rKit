cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()

set(PROJECT_EXTERNAL_TARGETS "")

macro(register_external_target target_name)
  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "External target '${target_name}' does not exist.")
  endif()
  list(FIND PROJECT_EXTERNAL_TARGETS "${target_name}" existing_index)
  if(existing_index EQUAL -1)
    list(APPEND PROJECT_EXTERNAL_TARGETS ${target_name})
  endif()
endmacro()

set(EXTERNAL_RULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
if(EXISTS "${EXTERNAL_RULES_DIR}")
  file(
    GLOB external_rule_files
    RELATIVE "${EXTERNAL_RULES_DIR}"
    "${EXTERNAL_RULES_DIR}/*.cmake"
  )

  list(SORT external_rule_files)
  foreach(rule_file IN LISTS external_rule_files)
    message("making external package '${rule_file}' available")
    include("${EXTERNAL_RULES_DIR}/${rule_file}")
  endforeach()
endif()

set(PROJECT_EXTERNAL_TARGETS "${PROJECT_EXTERNAL_TARGETS}" PARENT_SCOPE)
