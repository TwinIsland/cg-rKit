include(FetchContent)

# Eigen is header-only.
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
