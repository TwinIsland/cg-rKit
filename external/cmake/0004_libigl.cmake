if(USE_LIBIGL)

include(FetchContent)

set(LIBIGL_BUILD_TESTS OFF CACHE BOOL "Build libigl tests" FORCE)
set(LIBIGL_BUILD_TUTORIALS OFF CACHE BOOL "Build libigl tutorials" FORCE)
set(LIBIGL_INSTALL OFF CACHE BOOL "Install libigl targets" FORCE)
set(LIBIGL_USE_STATIC_LIBRARY OFF CACHE BOOL "Use libigl as static library" FORCE)

fetchcontent_declare(
  external_libigl
  GIT_REPOSITORY "https://github.com/libigl/libigl.git"
  GIT_TAG "v2.5.0"
  GIT_SHALLOW TRUE
)

fetchcontent_makeavailable(external_libigl)

if(TARGET igl::core)
  register_external_target(igl::core)
else()
  message(FATAL_ERROR "libigl fetch succeeded but no CMake target was exposed.")
endif()

endif() # USE_LIBIGL
