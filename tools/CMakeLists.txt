set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(TOOLS index extract vocabulary quantize search extract_and_quantize)
foreach(tool ${TOOLS})
    make_exec(${tool})
endforeach()

file(COPY ${PROJECT_SOURCE_DIR}/tools/filelist DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${PROJECT_SOURCE_DIR}/tools/sse DESTINATION ${CMAKE_BINARY_DIR}/bin/)

install (TARGETS ${TOOLS} RUNTIME DESTINATION /usr/local/bin)