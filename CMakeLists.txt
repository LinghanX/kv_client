cmake_minimum_required(VERSION 3.7)
project(kv_coordinator)

include_directories(include)
set(CMAKE_CXX_STANDARD 11)

add_executable(kv_coordinator main.cpp parse.h parse.cpp message.h message.cpp coordinator.cpp coordinator.h tcp_helper.cpp tcp_helper.h)