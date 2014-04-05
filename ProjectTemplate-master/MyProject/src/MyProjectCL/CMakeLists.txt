set (LIB MyProject)
set (APP ${LIB}CL)

set (AudioLIB AudioFileIO)
if (WITH_SNDLIB)
	set (SndLIB sndlib)
endif (WITH_SNDLIB)

# get all source files
file(GLOB ${APP}_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}/${LIB}/src/${APP} *.c;*.cpp;*.txt)
file(GLOB ${APP}_HEADERS RELATIVE ${CMAKE_SOURCE_DIR}/${LIB}/src/${APP} *.h;../incl/*.h;../../../incl/SharedSources/incl/*.h)

## add include directories
add_includes ()

add_executable(${APP} ${${APP}_SOURCES} ${${APP}_HEADERS})

# link the libraries
target_link_libraries(${APP} ${LIB} ${TestLIB} ${AudioLIB} ${SndLIB} ${OS_LIBS})
