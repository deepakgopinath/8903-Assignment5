function (add_directories)
    ## header of dependencies printoutmessage(STATUS "")
    message(STATUS "******************")
    message(STATUS "Targets added:")
     
    ## add dependencies from list and print out the name
    foreach(PATH ${PROJECT_DIRECTORIES})
        get_filename_component( FILENAME ${PATH}
                                NAME_WE)
        message(STATUS ${FILENAME})
        add_subdirectory(${PATH} "${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}")
    endforeach(PATH) 
     
    ## footer of dependencies printout
    message(STATUS "******************")
    message(STATUS "")
endfunction(add_directories)

function (add_includes)
    foreach(PATH ${PROJECT_INCLUDES})
        include_directories(${PATH})
    endforeach(PATH) 
endfunction (add_includes)

function (add_cppcheck_test)
	find_package(Cppcheck)
	if (CPPCHECK_FOUND)
		# copy list just to be sure
		list(APPEND CPPCHK_DEPLIST ${PROJECT_DIRECTORIES})

		# generate include list with "-I" prefix
		foreach(arg ${PROJECT_INCLUDES})
		   set(CPPCHK_INCL "${CPPCHK_INCL} -I ${arg}")
		endforeach(arg ${PROJECT_INCLUDES})

		# add the test itself!
		add_test(CppCheck ${CPPCHECK_EXECUTABLE} "--error-exitcode=1" ${CPPCHK_DEPLIST} ${CPPCHK_INCL})
	endif (CPPCHECK_FOUND)
endfunction (add_cppcheck_test)
