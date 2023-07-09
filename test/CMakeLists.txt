cmake_minimum_required(VERSION 3.0)

project(Test)

include_directories(..)
find_package(Threads REQUIRED)

add_executable(echo_server   echo_server.cc)
target_compile_options(echo_server PRIVATE -std=c++17)

add_executable(test_sigflow  test_sigflow.cc)
target_compile_options(test_sigflow PRIVATE -std=c++17)

add_executable(test_acceptor test_acceptor.cc)
target_compile_options(test_acceptor PRIVATE -std=c++17)

add_executable(test_timer    test_timer.cc)
target_compile_options(test_timer PRIVATE -std=c++17)
target_link_libraries(test_timer Threads::Threads)
