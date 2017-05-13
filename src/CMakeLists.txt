set(SOURCE_FILES
    featuresx.cpp
    options.cpp
    project.cpp
    reconstructor.cpp
    storage.cpp
    theia_utils.cpp
    utils.cpp
    )

set(HEADERS
    featuresx.h
    options.h
    project.h
    reconstructor.h
    storage.h
    theia_utils.h
    utils.h
    )

set(IO_SOURCE_FILES
    io/projectio.cpp
    io/report.cpp
    io/storageio.cpp
    )

set(IO_HEADERS
    io/projectio.h
    io/report.h
    io/storageio.h
    )

set(DESCRIPTOR_SOURCE_FILES
    descriptor/brisk_descriptor.cpp
    )

set(DESCRIPTOR_HEADERS
    descriptor/brisk_descriptor.h
    )

add_library(src ${SOURCE_FILES} ${HEADERS} ${IO_SOURCE_FILES} ${IO_HEADERS}
                ${DESCRIPTOR_SOURCE_FILES} ${DESCRIPTOR_HEADERS})

# Never include ui target here.
target_link_libraries(src ${THEIA_LIBRARIES} Qt5::Widgets ${OpenCV_LIBRARIES})
