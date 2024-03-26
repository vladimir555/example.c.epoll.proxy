cmake_minimum_required(VERSION 3.21)
project(example.c.epoll.proxy)

#set(CMAKE_C_STANDARD 17)
#set(CMAKE_C_STANDARD_REQUIRED ON)
#set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 17)
#set(CMAKE_C_FLAGS "-std=c17")
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "-std=c11 -O0 -ggdb")

set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -g -O0 -fsanitize=thread")


file(GLOB_RECURSE SOURCE
    ${CMAKE_SOURCE_DIR}/*.h
    ${CMAKE_SOURCE_DIR}/*.c
)

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCE})

include_directories(${CMAKE_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SOURCE})
