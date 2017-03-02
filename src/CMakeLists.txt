set(SOURCE_FILES
        project.cpp
        project.h
        storage.cpp
        storage.h
        features.cpp
        features.h
        utils.cpp
        utils.h)

add_library(src ${SOURCE_FILES})

# Never include ui target here.
target_link_libraries(src ${THEIA_LIBRARIES} Qt5::Widgets)
