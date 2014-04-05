# Install script for directory: /Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Assignment5/MyProject/cmake_install.cmake")
  INCLUDE("/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Assignment5/MyProjectCL/cmake_install.cmake")
  INCLUDE("/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Assignment5/JuceLibraryCode/cmake_install.cmake")
  INCLUDE("/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Assignment5/AudioFileIO/cmake_install.cmake")
  INCLUDE("/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Assignment5/TestCL/cmake_install.cmake")
  INCLUDE("/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Assignment5/Dsp/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Assignment5/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Assignment5/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
