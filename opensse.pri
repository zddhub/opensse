
INCLUDEPATH += $$PWD \
    /home/zdd/boost_1_55_0 \
    /home/opencv-2.4.8/include \

LIBS += -L/home/opencv-2.4.8/ \
            -lopencv_core -lopencv_imgproc -lopencv_highgui

#start with $$PWD, so that they will appear in included project
HEADERS += \
    $$PWD/features/feature.h \
    $$PWD/common/types.h \
    $$PWD/features/galif.h \
    $$PWD/features/detector.h \
    $$PWD/features/generator.h \
    $$PWD/features/util.h

SOURCES += \
    $$PWD/features/galif.cpp \
    $$PWD/features/detector.cpp \
    $$PWD/features/generator.cpp \
    $$PWD/features/util.cpp
