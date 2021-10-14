cmake_minimum_required(VERSION 3.20)
project(lab2)

set(CMAKE_CXX_STANDARD 14)

add_executable(lab2_pt_1 part_1.cpp part1.h)
add_executable(lab2_pt_2 part_2.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(lab2_pt_1 PRIVATE Threads::Threads)
target_link_libraries(lab2_pt_2 PRIVATE Threads::Threads)

