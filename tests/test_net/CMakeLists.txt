# need UNDEBUG for epoll
project(test_net)

CC_ADD_EXECUTABLES(LINK_LIBRARIES Threads::Threads ${LIB_NET})