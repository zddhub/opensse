set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_executable_cmd(index create_index)
add_executable_cmd(extract_and_quantize extract_and_quantize)
add_executable_cmd(extract extract_descriptors)
add_executable_cmd(vocabulary generate_vocabulary)
add_executable_cmd(quantize quantize)
add_executable_cmd(search sketch_search)

file(COPY ${PROJECT_SOURCE_DIR}/tools/generate_filelist/generate_filelist DESTINATION ${CMAKE_BINARY_DIR}/bin)