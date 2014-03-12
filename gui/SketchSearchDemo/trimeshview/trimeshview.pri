
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
