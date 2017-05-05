set(SOURCE_FILES
    cli.cpp
    )

set(HEADERS
    cli.h
    )

add_library(cli ${SOURCE_FILES} ${HEADERS})

# Never include ui target here.
target_link_libraries(cli src Qt5::Widgets)
