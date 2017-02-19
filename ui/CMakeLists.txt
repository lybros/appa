set(CMAKE_PREFIX_PATH $ENV{QT_CMAKE_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets REQUIRED)
INCLUDE_DIRECTORIES(Qt5Widgets_INCLUDE_DIRS)
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

set(SRC_UI
    mainwindow.cpp
    newproject.cpp
    )

set(HEADERS_UI
    mainwindow.h
    newproject.h
    )

set(FORMS_UI
    mainwindow.ui
    newproject.ui
    )

QT5_WRAP_UI(FORMS_UI_HDRS ${FORMS_UI})
QT5_ADD_RESOURCES(UI_RES resources.qrc)

add_library(ui
    ${SRC_UI}
    ${HEADERS_UI}
    ${FORMS_UI_HDRS}
    ${UI_RES}
    )

# We're building the ui library on top of src library,
# that's why need to mention it here.
# (and we will never specify ui target as a dependency of src target)
target_link_libraries(ui src Qt5::Widgets)
