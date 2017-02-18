cmake_minimum_required(VERSION 3.5)
project(cw_code)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(OPENIMAGEIO_LIBRARY $ENV{OPENIMAGEIO_LIBRARY})
set(OPENIMAGEIO_INCLUDE_DIR $ENV{OPENIMAGEIO_INCLUDE_DIR})

find_package(Theia REQUIRED)
include_directories(${THEIA_INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets REQUIRED)
INCLUDE_DIRECTORIES(Qt5Widgets_INCLUDE_DIRS)
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

set(SRC_UI ui/mainwindow.cpp)
set(HEADERS_UI ui/mainwindow.h)
set(FORMS_UI ui/mainwindow.ui)

set(SOURCE_FILES main.cpp src/project.h src/project.cpp)

QT5_WRAP_UI(FORMS_UI_HDRS ${FORMS_UI})
QT5_ADD_RESOURCES(UI_RES ui/resources.qrc)

add_executable(cw_code ${SOURCE_FILES} ${HEADERS_UI} ${SRC_UI} ${FORMS_UI_HDRS} ${UI_RES})
target_link_libraries(cw_code ${THEIA_LIBRARIES} Qt5::Widgets)
