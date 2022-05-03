cmake_minimum_required(VERSION 3.10)

project(knn VERSION 1.0)

FILE(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -Wall -O0")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O3 -march=native")

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)