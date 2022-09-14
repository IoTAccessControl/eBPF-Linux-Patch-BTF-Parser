cmake_minimum_required(VERSION 2.8)
PROJECT(test)

SET(CMAKE_BUILD_TYPE "RelWithDebInfo") # Debug Release RelWithDebInfo MinSizeRel
FILE(GLOB SOURCE_1 "${CMAKE_SOURCE_DIR}/src/btf_tool.c")
FILE(GLOB SOURCE_2 "${CMAKE_SOURCE_DIR}/main.cpp")

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/include/")
ADD_LIBRARY(btf_tool SHARED ${SOURCE_1})
LINK_LIBRARIES(btf_tool)
ADD_DEFINITIONS("-Wall -Werror") #  -std=c++11
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_2})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${btf_tool} PRIVATE btfparse)
ADD_SUBDIRECTORY("btfparse")