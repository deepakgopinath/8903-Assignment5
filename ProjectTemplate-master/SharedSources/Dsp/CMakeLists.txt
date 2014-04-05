set(CurrLib Dsp)

# get wrapper source files
file(GLOB ${CurrLib}_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}/SharedSources/Dsp *.c;*.cpp;*.txt;../../3rdPartyLibs/Fft/*.cpp)
file(GLOB ${CurrLib}_HEADERS RELATIVE ${CMAKE_SOURCE_DIR}/SharedSources/Dsp *.h;../incl/*.h;../../3rdPartyLibs/Fft/*.h)

## add include directories
foreach(PATH ${PROJECT_INCLUDES})
    include_directories(${PATH})
	include_directories(${CMAKE_SOURCE_DIR}/3rdPartyLibs/Fft)
endforeach(PATH) 

# build a static library
add_library(${CurrLib} STATIC ${${CurrLib}_SOURCES} ${${CurrLib}_HEADERS})
set_target_properties(${CurrLib} PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# set output name
set_target_properties(${CurrLib} PROPERTIES OUTPUT_NAME "${CurrLib}")
set_target_properties(${CurrLib} PROPERTIES PREFIX "lib")

if (WITH_CPPCHECKTARGET)
	# cppcheck target in the IDE
	file(GLOB to_remove RELATIVE ${CMAKE_SOURCE_DIR}/SharedSources/Dsp *.txt;../../3rdPartyLibs/Fft/rvfft.cpp)
	list(REMOVE_ITEM ${CurrLib}_SOURCES ${to_remove})
	include (GenerateCppcheck)
	GENERATE_CPPCHECK(SOURCES ${${CurrLib}_SOURCES}
						TARGET_NAME "CppCheck_${CurrLib}"
					  INCLUDES ${PROJECT_INCLUDES})
endif(WITH_CPPCHECKTARGET)