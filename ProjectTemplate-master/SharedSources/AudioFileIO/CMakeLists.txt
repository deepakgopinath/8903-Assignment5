set(CurrLib AudioFileIO)

## set version number
set (${CurrLib}_VERSION_MAJOR "0" CACHE INTERNAL "")
set (${CurrLib}_VERSION_MINOR "0" CACHE INTERNAL "")
set (${CurrLib}_VERSION_PATCH "1" CACHE INTERNAL "")

option (WITH_SNDLIB "Use sndlib for audiofile IO" ON)

# get wrapper source files
file(GLOB ${CurrLib}_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}/SharedSources/${CurrLib} *.c;*.cpp;*.txt)
file(GLOB ${CurrLib}_HEADERS RELATIVE ${CMAKE_SOURCE_DIR}/SharedSources/${CurrLib} *.h;../incl/*.h)

# get library source files
if(WITH_SNDLIB)
	add_subdirectory(${CMAKE_SOURCE_DIR}/3rdPartyLibs/sndlib "${CMAKE_CURRENT_BINARY_DIR}/sndlib")
	list (APPEND PROJECT_INCLUDES ${CMAKE_SOURCE_DIR}/3rdPartyLibs/sndlib)    
endif(WITH_SNDLIB)


## add include directories
foreach(PATH ${PROJECT_INCLUDES})
    include_directories(${PATH})
endforeach(PATH) 

# build a static library
add_library(${CurrLib} STATIC ${${CurrLib}_SOURCES} ${${CurrLib}_HEADERS})
set_target_properties(${CurrLib} PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# set output name
set_target_properties(${CurrLib} PROPERTIES OUTPUT_NAME "${CurrLib}")
set_target_properties(${CurrLib} PROPERTIES PREFIX "lib")

if(WITH_SNDLIB)
	if(MSVC)
		set_target_properties(${CurrLib} PROPERTIES COMPILE_FLAGS "/W1 /DMUS_WINDOZE")
	endif(MSVC)
endif(WITH_SNDLIB)
