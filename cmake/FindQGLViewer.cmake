# Try to find QGLViewer lib - http://libqglviewer.com/
# Once done this will define
#   - QGLVIEWER_FOUND
#   - QGLVIEWER_INCLUDE_DIRS
#   - QGLVIEWER_LIBRARIES

list(APPEND QGLVIEWER_CHECK_INCLUDE_DIRS
        ${QGLVIEWER_INCLUDE_DIR_HINTS}
        /usr/include
        /usr/local/include
        /opt/include
        /opt/local/include)

list(APPEND QGLVIEWER_CHECK_LIBRARY_DIRS
        ${QGLVIEWER_LIBRARY_DIR_HINTS}
        /usr/lib
        /usr/local/lib
        /opt/lib
        /opt/local/lib)

find_path(QGLVIEWER_INCLUDE_DIRS
        NAMES
        QGLViewer
        PATHS
        ${QGLVIEWER_CHECK_INCLUDE_DIRS})

find_library(QGLVIEWER_LIBRARIES
        NAMES
        QGLViewer
        PATHS
        ${QGLVIEWER_CHECK_LIBRARY_DIRS})

if(QGLVIEWER_INCLUDE_DIRS AND QGLVIEWER_LIBRARIES)
  set(QGLVIEWER_FOUND TRUE)
endif()

if(QGLVIEWER_FOUND)
    message(STATUS "Found required Appa dependency: QGLViewer in " ${QGLVIEWER_INCLUDE_DIRS})
else()
    if(QGLViewer_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find QGLViewer")
    endif()
endif()
