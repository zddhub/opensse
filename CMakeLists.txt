project(opensse)

cmake_minimum_required(VERSION 2.8.8)
cmake_policy(SET CMP0042 NEW)
set(HAVE_LIBPTHREAD YES)

include_directories(${PROJECT_SOURCE_DIR}/sse)
include_directories(/usr/local/include)

link_directories(/usr/local/lib)
set(REQUIRED_LIB opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui opencv_features2d opencv_ml boost_thread boost_system)

add_subdirectory(sse)

macro (add_executable_cmd arg)
	add_executable(${arg} ${arg}/main.cpp)
	target_link_libraries(${arg} ${REQUIRED_LIB} opensse)
endmacro (add_executable_cmd)

add_subdirectory(tools)
add_subdirectory(tests)

# install include
macro (install_head arg)
    file(GLOB HEADS sse/${arg}/*.h)
    install(FILES ${HEADS} DESTINATION /usr/local/include/opensse/${arg})
endmacro (install_head)

install_head(common)
install_head(io)
install_head(features)
install_head(quantize)
install_head(index)
install_head(search)
install_head(vocabulary)
install_head(.)

# install lib
macro (install_lib)
    file(GLOB LIBS ${opensse_BINARY_DIR}/lib/*)
    install(FILES ${LIBS} DESTINATION /usr/local/lib)
endmacro (install_lib)

install_lib()
