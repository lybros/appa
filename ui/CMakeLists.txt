# The Qt5OpenGL and Qt5Xml packages are required to avoid "QDomElement: no such
# file ot directory" error when compiling the project with QGlViewer library.
find_package(OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_library(QGLVIEWER_LIBRARIES QGLViewer)

find_package(Qt5Concurrent REQUIRED)

set(SRC_UI
        mainwindow.cpp
        newproject.cpp
        options_dialog.cpp
        process_manager.cpp
        reconstruction_window.cpp
        thumbnail_widget.cpp
        )

set(HEADERS_UI
        mainwindow.h
        newproject.h
        options_dialog.h
        process_manager.h
        reconstruction_window.h
        thumbnail_widget.h
        )

set(FORMS_UI
        mainwindow.ui
        newproject.ui
        options_dialog.ui
        )

QT5_WRAP_UI(FORMS_UI_HDRS ${FORMS_UI})

add_library(ui
        ${SRC_UI}
        ${HEADERS_UI}
        ${FORMS_UI_HDRS}
        )

# We're building the ui library on top of src library,
# that's why need to mention it here.
# (and we will never specify ui target as a dependency of src target)
target_link_libraries(ui src ${OPENGL_LIBRARIES} Qt5::Concurrent Qt5::Xml Qt5::OpenGL Qt5::Widgets ${QGLVIEWER_LIBRARIES})
