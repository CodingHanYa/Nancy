cmake_minimum_required(VERSION 3.0)

project(chatroom)
set(CMAKE_CXX_STANDARD 17)

include_directories(../..)
find_package(Threads REQUIRED)

add_executable(server server.cc)


