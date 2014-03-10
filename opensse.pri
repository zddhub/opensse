
INCLUDEPATH += $$PWD \
    /home/zdd/boost_1_55_0 \
    /home/opencv-2.4.8/include \

LIBS += -L/home/opencv-2.4.8/ \
            -lopencv_core -lopencv_imgproc -lopencv_highgui \
        -L/home/zdd/boost_1_55_0/stage/lib/ \
            -lboost_thread -lboost_system

#start with $$PWD, so that they will appear in included project
HEADERS += \
    $$PWD/common/types.h \
    $$PWD/features/feature.h \
    $$PWD/features/galif.h \
    $$PWD/features/detector.h \
    $$PWD/features/generator.h \
    $$PWD/features/util.h \
    $$PWD/io/filelist.h \
    $$PWD/io/reader_writer.h \
    $$PWD/common/distance.h \
    $$PWD/vocabulary/kmeans.h \
    $$PWD/vocabulary/kmeans_init.h \
    $$PWD/quantize/quantizer.h \
    $$PWD/index/invertedindex.h \
    $$PWD/index/tfidf.h

SOURCES += \
    $$PWD/features/galif.cpp \
    $$PWD/features/detector.cpp \
    $$PWD/features/generator.cpp \
    $$PWD/features/util.cpp \
    $$PWD/io/filelist.cpp \
    $$PWD/io/reader_writer.cpp \
    $$PWD/quantize/quantizer.cpp \
    $$PWD/index/invertedindex.cpp \
    $$PWD/index/tfidf.cpp
