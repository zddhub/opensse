project(opensse)

cmake_minimum_required(VERSION 2.8)

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

set(HAVE_LIBPTHREAD YES)

add_definitions(-std=c++11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/usr/local/include)
include_directories(..)
include_directories(${PROJECT_SOURCE_DIR}/sse)

link_directories(/usr/local/lib)
set(REQUIRED_LIB opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui opencv_features2d opencv_ml boost_thread boost_system)

add_subdirectory(sse)

add_subdirectory(tools)