include(FetchContent)

set(TBB_TEST OFF CACHE BOOL "Build oneTBB tests" FORCE)
set(TBB_EXAMPLES OFF CACHE BOOL "Build oneTBB examples" FORCE)
set(TBB_STRICT OFF CACHE BOOL "Enable oneTBB strict warnings" FORCE)
set(TBB4PY_BUILD OFF CACHE BOOL "Build oneTBB Python bindings" FORCE)
set(TBB_INSTALL OFF CACHE BOOL "Install oneTBB targets" FORCE)

fetchcontent_declare(
  external_tbb
  GIT_REPOSITORY "https://github.com/oneapi-src/oneTBB.git"
  GIT_TAG "v2021.13.0"
  GIT_SHALLOW TRUE
)

fetchcontent_makeavailable(external_tbb)

if(TARGET TBB::tbb)
  register_external_target(TBB::tbb)
elseif(TARGET tbb)
  register_external_target(tbb)
else()
  message(FATAL_ERROR "oneTBB fetch succeeded but no CMake target was exposed.")
endif()
