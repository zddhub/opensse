#-------------------------------------------------
#
# Project created by QtCreator 2014-03-12T08:55:19
#
#-------------------------------------------------

QT       += core gui \
            opengl \
            printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SketchSearchDemo
TEMPLATE = app

DESTDIR = ../bin

include(../../opensse.pri)
include(trimeshview/trimeshview.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    sketcharea.cpp \
    resultphotowidget.cpp \
    sketchsearcher.cpp \

HEADERS  += mainwindow.h \
    sketcharea.h \
    searchengine.h \
    resultphotowidget.h \
    sketchsearcher.h \
