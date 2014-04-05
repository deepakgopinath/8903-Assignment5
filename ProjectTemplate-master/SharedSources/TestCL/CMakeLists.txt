set (APP TestCL)

set (LIB MyProject)
set (AudioLIB AudioFileIO)
set (DspLIB Dsp)
if (WITH_SNDLIB)
	set (SndLIB sndlib)
endif (WITH_SNDLIB)

# with tests or not
option (WITH_TESTS "Use UnitTest++ for testing" ON)

# get library source files
if(WITH_TESTS)
	set (TestLIB UnitTest++)
	
	add_subdirectory(${CMAKE_SOURCE_DIR}/3rdPartyLibs/UnitTest++ "${CMAKE_CURRENT_BINARY_DIR}/UnitTest++")
	list (APPEND PROJECT_INCLUDES ${CMAKE_SOURCE_DIR}/3rdPartyLibs/UnitTest++)       

	## get all source files
	# this is the global main
	file(GLOB ${APP}_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}/SharedSources/${APP} Test.cpp)
	file(GLOB ${APP}_HEADERS RELATIVE ${CMAKE_SOURCE_DIR}/SharedSources/${APP} *.h;../incl/*.h)

	# these are the individual tests
	file(GLOB ${APP}_SOURCES_TEST RELATIVE ${CMAKE_SOURCE_DIR}/SharedSources/${APP} ../../Tests/*.cpp;)
	file(GLOB ${APP}_HEADERS_TEST RELATIVE ${CMAKE_SOURCE_DIR}/SharedSources/${APP} ../../Tests/*.h)
	source_group (SharedTests FILES ${APP}_SOURCES_TEST ${APP}_HEADERS_TEST)
	file(GLOB ${LIB}_SOURCES_TEST RELATIVE ${CMAKE_SOURCE_DIR}/SharedSources/${APP} ../../${LIB}/Tests/*.cpp;)
	file(GLOB ${LIB}_HEADERS_TEST RELATIVE ${CMAKE_SOURCE_DIR}/SharedSources/${APP} ../../${LIB}/Tests/*.h)
	source_group (MyProjectTests FILES ${LIB}_SOURCES_TEST ${LIB}_HEADERS_TEST)
	
	## add include directories
	add_includes ()

	add_executable(${APP} ${${APP}_SOURCES} ${${APP}_HEADERS} ${${APP}_SOURCES_TEST} ${${APP}_HEADERS_TEST} ${${LIB}_SOURCES_TEST} ${${LIB}_HEADERS_TEST})

	# link the libraries
	target_link_libraries(${APP} ${LIB} ${TestLIB} ${AudioLIB} ${SndLIB} ${OS_LIBS})
endif(WITH_TESTS)
