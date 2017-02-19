cmake_minimum_required(VERSION 2.9)

set(PROJECT_NAME cw_code)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(${PROJECT_NAME})

set(OPENIMAGEIO_LIBRARY $ENV{OPENIMAGEIO_LIBRARY})
set(OPENIMAGEIO_INCLUDE_DIR $ENV{OPENIMAGEIO_INCLUDE_DIR})

find_package(Theia REQUIRED)
include_directories(${THEIA_INCLUDE_DIRS})

# The order below is important:
add_subdirectory(src)
add_subdirectory(ui)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} src ui)
