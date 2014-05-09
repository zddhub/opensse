TEMPLATE = subdirs

SUBDIRS += \
    #opensse.pri \
    tests/test_galif \
    tests/test_reader_and_writer \
    tools/extract_descriptors \
    tools/generate_filelist \
    tools/generate_vocabulary \
    tools/quantize \
    tools/create_index \
    tools/sketch_search \
    gui/SketchSearchDemo \
    tests/test_similarity \
    tests/stat_vocab \
    tests/test_search \
    tools/extract_and_quantize

