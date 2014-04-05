
EXECUTE_PROCESS(COMMAND "/usr/local/bin/cppcheck"  "-I/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/MyProject/incl" "-I/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Assignment5/incl" "-I/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/SharedSources/incl"    --force --enable=style --enable=performance --inline-suppr --xml  "MyProject.cpp"
                RESULT_VARIABLE CPPCHECK_EXIT_CODE
                ERROR_VARIABLE ERROR_OUT
                WORKING_DIRECTORY "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/MyProject/src/MyProject")
IF(NOT CPPCHECK_EXIT_CODE EQUAL 0)
    MESSAGE(FATAL_ERROR "Error executing cppcheck for target , return code: ${CPPCHECK_EXIT_CODE}")
ENDIF()
IF(ERROR_OUT)
    MESSAGE("Detected errors:\n${ERROR_OUT}")
ENDIF()
FILE(WRITE "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Assignment5/cppcheck-report-CppCheck_MyProject.xml" "${ERROR_OUT}")
