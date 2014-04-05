

if (GTCMT_MACOSX)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -msse -msse2 -msse3")
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D NDEBUG")
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D DEBUG")
	set (CMAKE_OSX_ARCHITECTURES "i386;x86_64")
	
	## display XCode settings		
	mark_as_advanced (CMAKE_OSX_ARCHITECTURES)
	mark_as_advanced (CMAKE_OSX_SYSROOT)
	mark_as_advanced (CMAKE_OSX_DEPLOYMENT_TARGET)
endif(GTCMT_MACOSX)

if (GTCMT_LINUX32 OR GTCMT_LINUX64 OR MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -msse -msse2 -msse3")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif (GTCMT_LINUX32 OR GTCMT_LINUX64 OR MINGW)

if (MSVC)
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /fp:precise /EHsc /W4")
#	if (GTCMT_WIN32)
#		set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
#	else (GTCMT_WIN32)
#		set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
#	endif (GTCMT_WIN32)

	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /fp:precise /Ob2 /D NDEBUG")

	# option to switch between MT and MD builds
	option(BUILD_MD "use shared runtime libs (MD) instead of static libs (MT)" ON)
	if (BUILD_MD)
		set(RUNTIME_LIB_TYPE "MD" CACHE STRING "" FORCE)
	else (BUILD_MD)
		set(RUNTIME_LIB_TYPE "MT" CACHE STRING "" FORCE)
	endif (BUILD_MD)
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /${RUNTIME_LIB_TYPE}d")
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /${RUNTIME_LIB_TYPE}")
endif (MSVC)
