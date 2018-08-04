if (BUILD_TESTING)
	enable_testing ()

	if (APPLE OR UNIX)
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -ansi -pedantic -fdiagnostics-color=always")
		set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -Og")
		set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Ofast")
	endif ()

	add_executable (test-clist test-basic.c test-multi.c)
	target_link_libraries (test-clist autotest)
	add_test (NAME clist-test COMMAND "$<TARGET_FILE:test-clist>")
endif ()
