cmake_minimum_required(VERSION 3.5)
project(cw_code)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(OPENIMAGEIO_LIBRARY $ENV{OPENIMAGEIO_LIBRARY})
set(OPENIMAGEIO_INCLUDE_DIR $ENV{OPENIMAGEIO_INCLUDE_DIR})

find_package(Theia REQUIRED)
include_directories(${THEIA_INCLUDE_DIRS})

set(SOURCE_FILES main.cpp)
add_executable(cw_code ${SOURCE_FILES})
target_link_libraries(cw_code ${THEIA_LIBRARIES})