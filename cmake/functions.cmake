#function(prebuild_check TARGET)
#    message("-- Running check.py on: ${CMAKE_CURRENT_SOURCE_DIR} with ${Python3_EXECUTABLE}")
#    execute_process(COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/check.py ${CMAKE_CURRENT_SOURCE_DIR} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
#    add_custom_command(TARGET PRE_BUILD ${TARGET}${TARGET} COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/check.py ${CMAKE_CURRENT_SOURCE_DIR} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )
#    #add_custom_command(TARGET ${TARGET} COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/check.py ${CMAKE_CURRENT_SOURCE_DIR} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} DEPENDS ${TARGET})
#                                                                                                                                                                    #^ error
#endfunction()


#included in build tree and retriggered only when script changes or file has changed
function(prebuild_check target)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    set(stamp "${CMAKE_CURRENT_BINARY_DIR}/${target}_precheck.stamp")

    add_custom_command(
            OUTPUT "${stamp}"
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/check.py" "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E touch "${stamp}"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            DEPENDS "${CMAKE_SOURCE_DIR}/check.py"
    )

    add_custom_target("${target}_precheck" DEPENDS "${stamp}")
    add_dependencies("${target}" "${target}_precheck")

    message(STATUS "Prebuild check wired for target: ${target}")
endfunction()