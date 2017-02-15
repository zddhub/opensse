project(opensse)

cmake_minimum_required(VERSION 2.8.8)
cmake_policy(SET CMP0042 NEW)
set(HAVE_LIBPTHREAD YES)

add_subdirectory(sse)

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
