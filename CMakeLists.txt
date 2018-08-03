project (clist)
cmake_minimum_required (VERSION 3.2)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

if (BUILD_TESTING)
	enable_testing()
	add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/ext/autotest")
	add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif ()
