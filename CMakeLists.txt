project(steiner)

cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 14)

find_package(Boost 1.58.0 REQUIRED COMPONENTS graph)

aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})
