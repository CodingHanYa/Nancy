#include <sys/signal.h>
#include <sys/socket.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <functional>
#include <mutex>

#include <nancy/net/fd.h>
#include <nancy/net/details/signal.h>
// ======================
//        signal
// ======================

static int pipe_fd[2] = {0};
static const int IN = 1;
static const int OUT = 0;
static std::mutex signal_lock;
static void signal_handler(int sig);


/**
 * @brief 初始化信号处理机制，若重入该函数，返回-1
 * @return 可供监听的套接字
 */
int nc::net::signal_socket_init() {
    std::lock_guard<std::mutex> lock(signal_lock);
    if (pipe_fd[0] != 0 || pipe_fd[1] != 0) {
        return -1;
    }
    if (-1 == socketpair(PF_UNIX, SOCK_STREAM, 0, pipe_fd)) {
        throw std::logic_error("Can't init signal pipe");
    }
    nc::net::set_nonblocking(pipe_fd[1]);
    return pipe_fd[OUT];
}

void nc::net::signal_add(int sig) {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = signal_handler;
    sa.sa_flags |= SA_RESTART;  
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}


void nc::net::signal_socket_close() {
    if (pipe_fd[0]) close(pipe_fd[0]);
    if (pipe_fd[1]) close(pipe_fd[1]);
}

void signal_handler(int sig) {
    int save_errno = errno;
    int msg = sig;
    send(pipe_fd[IN], (char*)&msg, 1, 0);
    errno = save_errno;
}
