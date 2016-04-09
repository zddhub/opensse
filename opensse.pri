#-------------------------------------------------------------------------
# Copyright (c) 2014 Zhang Dongdong
# All rights reserved.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#-------------------------------------------------------------------------

unix:!macx {
# I use Ubuntu
INCLUDEPATH += $$PWD \
    /usr/local/include

LIBS += -L/usr/local/lib/ \
            -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lopencv_features2d -lopencv_ml \
            -lboost_thread -lboost_system
}

macx {

INCLUDEPATH += $$PWD \
    /usr/local/include

LIBS += -L/usr/local/lib/ \
            -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lopencv_features2d -lopencv_ml \
            -lboost_thread -lboost_system

#for commind line
CONFIG -= app_bundle

#change clang version for you mac
#message("QMAKE_MAC_SDK:" $$QMAKE_MAC_SDK)
}


#start with $$PWD, so that they will appear in included project
HEADERS += \
    $$PWD/sse/common/types.h \
    $$PWD/sse/features/feature.h \
    $$PWD/sse/features/galif.h \
    $$PWD/sse/features/detector.h \
    $$PWD/sse/features/generator.h \
    $$PWD/sse/features/util.h \
    $$PWD/sse/io/filelist.h \
    $$PWD/sse/io/reader_writer.h \
    $$PWD/sse/common/distance.h \
    $$PWD/sse/vocabulary/kmeans.h \
    $$PWD/sse/vocabulary/kmeans_init.h \
    $$PWD/sse/quantize/quantizer.h \
    $$PWD/sse/index/invertedindex.h \
    $$PWD/sse/index/tfidf.h

SOURCES += \
    $$PWD/sse/features/galif.cpp \
    $$PWD/sse/features/detector.cpp \
    $$PWD/sse/features/generator.cpp \
    $$PWD/sse/features/util.cpp \
    $$PWD/sse/io/filelist.cpp \
    $$PWD/sse/io/reader_writer.cpp \
    $$PWD/sse/quantize/quantizer.cpp \
    $$PWD/sse/index/invertedindex.cpp \
    $$PWD/sse/index/tfidf.cpp
