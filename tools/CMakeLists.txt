set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_executable_cmd(create_index)
add_executable_cmd(extract_and_quantize)
add_executable_cmd(extract_descriptors)
add_executable_cmd(generate_vocabulary)
add_executable_cmd(quantize)
add_executable_cmd(sketch_search)

