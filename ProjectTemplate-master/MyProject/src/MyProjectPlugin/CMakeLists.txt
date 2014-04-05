set(CurrLib MyProjectPlugin)

# get all source files
set(LocalDir ${CMAKE_SOURCE_DIR}/MyProject/src/${CurrLib})
file(GLOB ${CurrLib}_SOURCES RELATIVE ${LocalDir} *.cpp;*.txt)
file(GLOB ${CurrLib}_HEADERS RELATIVE ${LocalDir} *.h;../../incl/*.h)

## add include directories
add_includes ()

option (PLUGIN_VST "build a VST plugin" OFF)
if (APPLE)
	option (PLUGIN_AU "build an AU plugin" OFF)
endif (APPLE)

if (PLUGIN_VST OR PLUGIN_AU)
if (PLUGIN_VST)
	find_package(VST REQUIRED)

	if (VST_FOUND)
		include_directories(${VST_INCLUDE_DIR})
		file(GLOB VST_SOURCES RELATIVE ${LocalDir} ${JUCE_PLUGIN_WRAPPER_DIR}/VST/*.cpp)
		file(GLOB VST_HEADERS RELATIVE ${LocalDir} ${JUCE_PLUGIN_WRAPPER_DIR}/VST/*.h;)
		if (APPLE)
			file(GLOB VST_SOURCES_MAC RELATIVE ${LocalDir} ${JUCE_PLUGIN_WRAPPER_DIR}/VST/*.mm)
		endif (APPLE)
	endif (VST_FOUND)
endif (PLUGIN_VST)
if (PLUGIN_AU)
	set(COREAUDIO_SDK_PATH "/Developer/Extras/CoreAudio")
	file(GLOB AU_SOURCES RELATIVE ${LocalDir} 
		${JUCE_PLUGIN_WRAPPER_DIR}/AU/*.mm;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/OtherBases/AUOutputBase.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/Utility/AUInputFormatConverter.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/Utility/AUBuffer.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUCarbonViewBase/CarbonEventHandler.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/ComponentBase.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/AUInputElement.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/AUScopeElement.cpp;
		${COREAUDIO_SDK_PATH}/PublicUtility/CAAUParameter.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUCarbonViewBase/AUCarbonViewBase.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/AUBase.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/AUBase.h;
		${COREAUDIO_SDK_PATH}/PublicUtility/CAVectorUnit.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/Utility/AUSilentTimeout.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUCarbonViewBase/AUCarbonViewControl.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/Utility/AUDebugDispatcher.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/OtherBases/AUOutputBase.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUViewBase/AUViewLocalizedStringKeys.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUCarbonViewBase/CarbonEventHandler.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUCarbonViewBase/AUCarbonViewBase.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/AUInputElement.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/Utility/AUTimestampGenerator.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/OtherBases/AUMIDIEffectBase.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/OtherBases/AUMIDIBase.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/OtherBases/MusicDeviceBase.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/AUScopeElement.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUCarbonViewBase/AUCarbonViewControl.cpp;
		${COREAUDIO_SDK_PATH}/PublicUtility/CAVectorUnitTypes.h;
		${COREAUDIO_SDK_PATH}/PublicUtility/CAAudioChannelLayout.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/Utility/AUBuffer.cpp;
		${COREAUDIO_SDK_PATH}/PublicUtility/CAAudioChannelLayout.cpp;
		${COREAUDIO_SDK_PATH}/PublicUtility/CAVectorUnit.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/OtherBases/AUMIDIEffectBase.h;
		${COREAUDIO_SDK_PATH}/PublicUtility/CAStreamBasicDescription.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/AUOutputElement.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/AUOutputElement.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/ComponentBase.cpp;
		${COREAUDIO_SDK_PATH}/PublicUtility/CAMutex.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/Utility/AUDebugDispatcher.cpp;
		${COREAUDIO_SDK_PATH}/PublicUtility/CAAUParameter.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUCarbonViewBase/AUCarbonViewDispatch.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/OtherBases/AUEffectBase.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/OtherBases/AUEffectBase.h;
		${COREAUDIO_SDK_PATH}/PublicUtility/CAMutex.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/AUDispatch.cpp;
		${COREAUDIO_SDK_PATH}/PublicUtility/CAStreamBasicDescription.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/AUDispatch.h;
		${COREAUDIO_SDK_PATH}/PublicUtility/CADebugMacros.h;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/OtherBases/AUMIDIBase.cpp;
		${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/OtherBases/MusicDeviceBase.cpp;
		)
	include_directories(${COREAUDIO_SDK_PATH}/PublicUtility/)
	include_directories(${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/Utility/)
	include_directories(${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase/)
	include_directories(${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUViewBase/)
	include_directories(${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/OtherBases/)
	include_directories(${COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUCarbonViewBase/)
endif (PLUGIN_AU)

file(GLOB UTILITY_SOURCES RELATIVE ${LocalDir} ${JUCE_PLUGIN_WRAPPER_DIR}/utility/*.cpp)
file(GLOB UTILITY_HEADERS RELATIVE ${LocalDir} ${JUCE_PLUGIN_WRAPPER_DIR}/utility/*.h;)	

set(JUCE_PLUGIN_SOURCES ${VST_SOURCES} ${VST_SOURCES_MAC} ${AU_SOURCES} ${UTILITY_SOURCES})
set(JUCE_PLUGIN_HEADERS ${VST_HEADERS} ${AU_HEADERS} ${UTILITY_HEADERS})

# build the library
if (APPLE)
	set(PluginInfoPlist ${CMAKE_BINARY_DIR}/res/Info.plist)
	add_library(${CurrLib} MODULE ${${CurrLib}_SOURCES} ${${CurrLib}_HEADERS} ${JUCE_PLUGIN_SOURCES} ${JUCE_PLUGIN_HEADERS})
    set_target_properties(${CurrLib} PROPERTIES
        BUNDLE 1
        XCODE_ATTRIBUTE_MACH_O_TYPE mh_bundle
        BUNDLE_EXTENSION component
        XCODE_ATTRIBUTE_WRAPPER_EXTENSION component
        XCODE_ATTRIBUTE_INFOPLIST_FILE ${PluginInfoPlist}
        MACOSX_BUNDLE_INFO_PLIST ${PluginInfoPlist}
        XCODE_ATTRIBUTE_GENERATE_PKGINFO_FILE "YES"   
	)     
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO})
else (APPLE)
	add_library(${CurrLib} SHARED ${${CurrLib}_SOURCES} ${${CurrLib}_HEADERS} ${JUCE_PLUGIN_SOURCES} ${JUCE_PLUGIN_HEADERS})
endif(APPLE)
set_target_properties(${CurrLib} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${CurrLib} PROPERTIES OUTPUT_NAME "${CurrLib}")

target_link_libraries(${CurrLib} MyProject JuceLibraryCode ${OS_LIBS})

if (WITH_CPPCHECKTARGET)
	# cppcheck target in the IDE
	file(GLOB to_remove RELATIVE ${CMAKE_SOURCE_DIR}/${CurrLib}/src/${CurrLib} *.txt)
	list(REMOVE_ITEM ${CurrLib}_SOURCES ${to_remove})
	include (GenerateCppcheck)
	GENERATE_CPPCHECK(SOURCES ${${CurrLib}_SOURCES}
						TARGET_NAME "CppCheck_${CurrLib}"
					  INCLUDES ${PROJECT_INCLUDES})
endif(WITH_CPPCHECKTARGET)
endif (PLUGIN_VST OR PLUGIN_AU)