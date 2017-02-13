project(opensse)

cmake_minimum_required(VERSION 2.8.8)
cmake_policy(SET CMP0042 NEW)
set(HAVE_LIBPTHREAD YES)

include_directories(.)
include_directories(${PROJECT_SOURCE_DIR}/include)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)
set(REQUIRED_LIB opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui opencv_features2d opencv_ml boost_thread boost_system)

set(
    SOURCES
    io/filelist.cpp
    io/reader_writer.cpp
    features/util.cpp
    features/generator.cpp
    features/detector.cpp
    features/galif.cpp
    quantize/quantizer.cpp
    index/tfidf.cpp
    index/invertedindex.cpp
    )

add_library(opensse SHARED ${SOURCES})
target_link_libraries(opensse ${REQUIRED_LIB})
set(LIBRARY_OUTPUT_PATH ${opensse_BINARY_DIR}/lib)

# install include
macro (install_head arg)
    file(GLOB HEADS ${arg}/*.h)
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
