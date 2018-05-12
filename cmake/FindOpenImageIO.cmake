# Try to find OpenImageIO lib - https://github.com/OpenImageIO/oiio
# Once done this will define
#   - OPENIMAGEIO_FOUND
#   - OPENIMAGEIO_INCLUDE_DIRS
#   - OPENIMAGEIO_LIBRARIES

list(APPEND OPENIMAGEIO_CHECK_INCLUDE_DIRS
        ${OPENIMAGEIO_INCLUDE_DIR_HINTS}
        /usr/include
        /usr/local/include
        /opt/include
        /opt/local/include)

list(APPEND OPENIMAGEIO_CHECK_LIBRARY_DIRS
        ${OPENIMAGEIO_LIBRARY_DIR_HINTS}
        /usr/lib
        /usr/local/lib
        /opt/lib
        /opt/local/lib)

find_path(OPENIMAGEIO_INCLUDE_DIRS
        NAMES
        OpenImageIO
        PATHS
        ${OPENIMAGEIO_CHECK_INCLUDE_DIRS})

find_library(OPENIMAGEIO_LIBRARIES
        NAMES
        OpenImageIO
        PATHS
        ${OPENIMAGEIO_CHECK_LIBRARY_DIRS})

if(OPENIMAGEIO_INCLUDE_DIRS AND OPENIMAGEIO_LIBRARIES)
  set(OPENIMAGEIO_FOUND TRUE)
endif()

if(OPENIMAGEIO_FOUND)
    message(STATUS "Found required Appa dependency: OpenImageIO in " ${OPENIMAGEIO_INCLUDE_DIRS})
else()
    if(OpenImageIO_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find OpenImageIO")
    endif()
endif()
