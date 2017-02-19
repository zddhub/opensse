set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/tests)

macro (add_executable_cmd exec project)
    add_executable(${exec} ${project}/main.cpp)
    target_link_libraries(${exec} ${REQUIRED_LIB} opensse)
endmacro (add_executable_cmd)

add_executable_cmd(stat_vocab stat_vocab)
add_executable_cmd(test_galif test_galif)
add_executable_cmd(test_reader_and_writer test_reader_and_writer)
# add_executable_cmd(test_search test_search)
add_executable_cmd(test_similarity test_similarity)