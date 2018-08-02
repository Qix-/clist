if (BUILD_TESTING)
	enable_testing ()

	if (APPLE OR UNIX)
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pedantic -fdiagnostics-color=always -fvisibility=hidden")
		set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -Og")
		set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Ofast")
	endif ()

	include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../ext/qunit")

	add_executable (test-clist-ansi syntax-check.c) # does not need to be run, just needs to compile
	target_compile_options (test-clist-ansi PRIVATE -ansi)

	add_executable (test-clist test-basic.c)
	target_compile_options (test-clist PRIVATE -std=c99)

	add_test (NAME clist-test COMMAND "$<TARGET_FILE:test-clist>")

	add_executable (test-clist-multi test-multi.c)
	target_compile_options (test-clist-multi PRIVATE -std=c99)

	add_test (NAME clist-multi-test COMMAND "$<TARGET_FILE:test-clist-multi>")
endif ()
