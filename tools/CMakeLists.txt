set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(TOOLS index extract vocabulary quantize search extract_and_quantize)
set(SCRIPT_TOOLS sse filelist)

macro (make_exec arg)
    add_executable(${arg} ${arg}.cpp)
    target_link_libraries(${arg} ${REQUIRED_LIB} opensse)
endmacro (make_exec)

foreach(tool ${TOOLS})
    make_exec(${tool})
endforeach()

file(COPY ${PROJECT_SOURCE_DIR}/tools/filelist DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${PROJECT_SOURCE_DIR}/tools/sse DESTINATION ${CMAKE_BINARY_DIR}/bin/)

install (TARGETS ${TOOLS} RUNTIME DESTINATION /usr/local/bin)

install (FILES ${SCRIPT_TOOLS} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ DESTINATION /usr/local/bin)