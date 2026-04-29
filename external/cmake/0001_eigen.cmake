if(USE_EIGEN)

include(FetchContent)

set(BUILD_TESTING OFF CACHE BOOL "Build Eigen tests" FORCE)
set(EIGEN_BUILD_DOC OFF CACHE BOOL "Build Eigen documentation" FORCE)
set(EIGEN_BUILD_PKGCONFIG OFF CACHE BOOL "Build Eigen pkg-config file" FORCE)

fetchcontent_declare(
  external_eigen
  GIT_REPOSITORY "https://gitlab.com/libeigen/eigen.git"
  GIT_TAG "3.4.0"
  GIT_SHALLOW TRUE
)

fetchcontent_makeavailable(external_eigen)

if(TARGET Eigen3::Eigen)
  register_external_target(Eigen3::Eigen)
else()
  message(FATAL_ERROR "Eigen fetch succeeded but no CMake target was exposed.")
endif()

endif() # USE_EIGEN
