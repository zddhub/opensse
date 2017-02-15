include_directories(${PROJECT_SOURCE_DIR}/sse)
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