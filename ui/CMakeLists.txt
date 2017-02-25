find_package(Qt53DCore REQUIRED)
find_package(Qt53DRender)
find_package(Qt53DExtras)
find_package(Qt53DInput)

set(SRC_UI
    mainwindow.cpp
    newproject.cpp
    reconstruction_window.cpp
    )

set(HEADERS_UI
    mainwindow.h
    newproject.h
    reconstruction_window.h
    )

set(FORMS_UI
    mainwindow.ui
    newproject.ui
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
target_link_libraries(ui src Qt5::Widgets Qt5::3DCore Qt5::3DExtras Qt5::3DRender Qt5::3DInput)
