set(
    SOURCES
    io/filelist.cpp
    io/reader_writer.cpp
    io/json_parser.cpp
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

install (TARGETS opensse LIBRARY DESTINATION /usr/local/lib)

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