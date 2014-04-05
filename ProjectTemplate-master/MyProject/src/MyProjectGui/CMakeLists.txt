set(APP MyProjectGui)

if (WITH_JUCE)
	# get all source files
	file(GLOB ${APP}_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}/MyProject/src/${APP} *.cpp;*.txt)
	file(GLOB ${APP}_HEADERS RELATIVE ${CMAKE_SOURCE_DIR}/MyProject/src/${APP} *.h;../../incl/*.h)

	## add include directories
	add_includes ()

	if (GTCMT_WIN32 OR GTCMT_WIN64)
		add_executable(${APP} WIN32 ${${APP}_SOURCES} ${${APP}_HEADERS} ${${APP}_RESOURCES})
	else (GTCMT_WIN32 OR GTCMT_WIN64)
		add_executable(${APP} MACOSX_BUNDLE ${${APP}_SOURCES} ${${APP}_HEADERS} ${${APP}_RESOURCES})

		set_target_properties(${APP} PROPERTIES XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING "YES")
		set_target_properties(${APP} PROPERTIES XCODE_ATTRIBUTE_DEAD_CODE_STRIPPING "YES")

		# set various propertiesin the Info.plist file
		set (VERSION_STRING "${MyProject_VERSION_MAJOR}.${MyProject_VERSION_MINOR}.${MyProject_VERSION_PATCH}")

		set_target_properties(${APP} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "${APP}")
		set_target_properties(${APP} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION_STRING}")
		set_target_properties(${APP} PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION_STRING}")
	endif (GTCMT_WIN32 OR GTCMT_WIN64)

	# set output name
	set_target_properties(${APP} PROPERTIES OUTPUT_NAME "${APP}")

	target_link_libraries(${APP} JuceLibraryCode MyProject ${OS_LIBS})

	if (WITH_CPPCHECKTARGET)
		# cppcheck target in the IDE
		file(GLOB to_remove RELATIVE ${CMAKE_SOURCE_DIR}/${APP}/src/${APP} *.txt)
		list(REMOVE_ITEM ${APP}_SOURCES ${to_remove})
		include (GenerateCppcheck)
		GENERATE_CPPCHECK(SOURCES ${${APP}_SOURCES}
							TARGET_NAME "CppCheck_${APP}"
						  INCLUDES ${PROJECT_INCLUDES})
	endif(WITH_CPPCHECKTARGET)
endif (WITH_JUCE)