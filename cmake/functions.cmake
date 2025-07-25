function(prebuild_check TARGET)
    message("-- Running check.py on: ${CMAKE_CURRENT_SOURCE_DIR} with ${Python3_EXECUTABLE}")
    execute_process(COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/check.py ${CMAKE_CURRENT_SOURCE_DIR} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    add_custom_command(TARGET ${TARGET} COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/check.py ${CMAKE_CURRENT_SOURCE_DIR} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} DEPENDS ${TARGET})
endfunction()