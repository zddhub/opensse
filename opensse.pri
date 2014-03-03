
INCLUDEPATH += /home/zdd/boost_1_55_0 \
    /home/opencv-2.4.8/include \

LIBS += -L/home/opencv-2.4.8/ \
            -lopencv_core -lopencv_imgproc -lopencv_highgui

HEADERS += \
    features/feature.h \
    common/types.h \
    features/galif.h \
    features/detector.h

SOURCES += \
    features/galif.cpp \
    features/detector.cpp
