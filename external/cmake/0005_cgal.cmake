if(USE_CGAL)

find_package(CGAL CONFIG REQUIRED)

if(TARGET CGAL::CGAL)
  register_external_target(CGAL::CGAL)
else()
  message(FATAL_ERROR "CGAL was found but target CGAL::CGAL is not available.")
endif()

endif() # USE_CGAL
