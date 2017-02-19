set(SOURCE_FILES
    project.h
    project.cpp
    )

add_library(src ${SOURCE_FILES})

# Never include ui target here.
target_link_libraries(src ${THEIA_LIBRARIES})
