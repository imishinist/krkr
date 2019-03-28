cmake_minimum_required(VERSION 3.12)
project(krkr)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR})
file(GLOB SRC_FILES *.cpp *.h)

add_executable(krkr ${SRC_FILES})