project(steiner)

cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 14)

aux_source_directory(. SRC_LIST)

find_package(Boost 1.58.0 REQUIRED COMPONENTS graph)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(${PROJECT_NAME} ${SRC_LIST})
endif()
