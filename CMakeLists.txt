cmake_minimum_required(VERSION 2.9)

set(PROJECT_NAME appa)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(${PROJECT_NAME})

set(OPENIMAGEIO_LIBRARY $ENV{OPENIMAGEIO_LIBRARY})
set(OPENIMAGEIO_INCLUDE_DIR $ENV{OPENIMAGEIO_INCLUDE_DIR})

set(QGLVIEWER_INCLUDE_DIRS /usr/local/include/QGLViewer)
include_directories(${QGLVIEWER_INCLUDE_DIRS})

find_package(Theia REQUIRED)
include_directories(${THEIA_INCLUDE_DIRS})

set(CMAKE_PREFIX_PATH $ENV{QT_CMAKE_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets REQUIRED)
include_directories(Qt5Widgets_INCLUDE_DIRS)
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

# The order below is important:
add_subdirectory(libs/QtAwesome/QtAwesome)
add_subdirectory(src)
add_subdirectory(ui)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} src ui QtAwesome)
