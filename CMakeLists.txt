cmake_minimum_required(VERSION 2.9)

set(PROJECT_NAME cw_code)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_PREFIX_PATH $ENV{QT_CMAKE_DIR})

project(${PROJECT_NAME})

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

set(SRC_UI
    ui/mainwindow.cpp
    ui/newproject.cpp
    )

set(HEADERS_UI
    ui/mainwindow.h
    ui/newproject.h
    )

set(FORMS_UI
    ui/mainwindow.ui
    ui/newproject.ui
    )

set(SOURCE_FILES main.cpp src/project.h src/project.cpp)

QT5_WRAP_UI(FORMS_UI_HDRS ${FORMS_UI})
QT5_ADD_RESOURCES(UI_RES ui/resources.qrc)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADERS_UI} ${SRC_UI} ${FORMS_UI_HDRS} ${UI_RES})
target_link_libraries(${PROJECT_NAME} ${THEIA_LIBRARIES} Qt5::Widgets)
