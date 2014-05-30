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

INCLUDEPATH += $$PWD \
               $$PWD/include

HEADERS += \
    trimeshview/trimeshview.h

SOURCES += \
    trimeshview/trimeshview.cpp \
    trimeshview/trimeshview_draw_ridges_and_valleys.cpp \
    trimeshview/trimeshview_draw_lines.cpp \
    trimeshview/trimeshview_draw_base.cpp \
    trimeshview/trimeshview_draw_apparent_ridges.cpp \
    trimeshview/trianglemesh/trianglemesh_tstrips.cpp \
    trimeshview/trianglemesh/trianglemesh_stats.cpp \
    trimeshview/trianglemesh/trianglemesh_pointareas.cpp \
    trimeshview/trianglemesh/trianglemesh_normals.cpp \
    trimeshview/trianglemesh/trianglemesh_io.cpp \
    trimeshview/trianglemesh/trianglemesh_curvature.cpp \
    trimeshview/trianglemesh/trianglemesh_connectivity.cpp \
    trimeshview/trianglemesh/trianglemesh_bounding.cpp \
    trimeshview/trianglemesh/KDtree.cpp \
    trimeshview/trianglemesh/GLCamera.cpp
