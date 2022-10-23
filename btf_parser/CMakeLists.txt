cmake_minimum_required(VERSION 2.8)
PROJECT(getBTF)

SET(CMAKE_BUILD_TYPE "RelWithDebInfo") # Debug Release RelWithDebInfo MinSizeRel
FILE(GLOB SRC "${CMAKE_SOURCE_DIR}/main.c"
	"${CMAKE_SOURCE_DIR}/src/wrapper.cpp"
	"${CMAKE_SOURCE_DIR}/src/utils.cpp")

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/include/")
ADD_DEFINITIONS("-Wall -Werror -lstdc++") #  -std=c++11
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE btfparse)
ADD_SUBDIRECTORY("../deps/btfparse" libbtfparse)
