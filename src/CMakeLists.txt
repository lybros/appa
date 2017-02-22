set(SOURCE_FILES
    project.h
    project.cpp
    storage.h
    storage.cpp
    )

add_library(src ${SOURCE_FILES})

# Never include ui target here.
target_link_libraries(src ${THEIA_LIBRARIES} Qt5::Widgets)
