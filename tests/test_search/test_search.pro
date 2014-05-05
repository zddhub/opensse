TEMPLATE = app
CONFIG += console

DESTDIR = ../bin

include(../../opensse.pri)

SOURCES += main.cpp \
    sketchsearcher.cpp

HEADERS += \
    sketchsearcher.h \
    searchengine.h

