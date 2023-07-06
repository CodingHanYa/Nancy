cmake_minimum_required(VERSION 3.0)

project(Test)
set(CMAKE_CXX_STANDARD 17)

include_directories(..)
find_package(Threads REQUIRED)

add_executable(echo_server   echo_server.cc)
add_executable(test_sigflow  test_sigflow.cc)
add_executable(test_acceptor test_acceptor.cc)
add_executable(test_smachine test_smachine.cc)
# add_executable(test_http test_http.cc)

add_executable(test_timer    test_timer.cc)
target_link_libraries(test_timer Threads::Threads)
