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
