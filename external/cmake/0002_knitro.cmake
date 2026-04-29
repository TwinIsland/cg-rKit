if(USE_KNITRO)

set(KNITRO_PATH "/opt/artelys/knitro-15.0.1-Linux64" CACHE PATH "Path to the Knitro installation")
set(KNITRO_INCLUDE_DIR "${KNITRO_PATH}/include")
set(KNITRO_LIBRARY "${KNITRO_PATH}/lib/libknitro1501.so")

if(NOT EXISTS "${KNITRO_INCLUDE_DIR}/knitro.h")
  message(FATAL_ERROR "Knitro header not found at ${KNITRO_INCLUDE_DIR}/knitro.h")
endif()

if(NOT EXISTS "${KNITRO_LIBRARY}")
  message(FATAL_ERROR "Knitro shared library not found at ${KNITRO_LIBRARY}")
endif()

add_library(knitro1501 SHARED IMPORTED GLOBAL)
set_target_properties(
  knitro1501
  PROPERTIES
    IMPORTED_LOCATION "${KNITRO_LIBRARY}"
)

add_library(project_knitro INTERFACE)
target_include_directories(project_knitro INTERFACE "${KNITRO_INCLUDE_DIR}")
target_compile_definitions(project_knitro INTERFACE KNITRO_PATH="${KNITRO_PATH}")
target_link_libraries(project_knitro INTERFACE knitro1501)
set_target_properties(
  project_knitro
  PROPERTIES
    ARTELYS_LICENSE_FILE "${KNITRO_LICENSE_FILE}"
)

register_external_target(project_knitro)

endif() # USE_KNITRO
