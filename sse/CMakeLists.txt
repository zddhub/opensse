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