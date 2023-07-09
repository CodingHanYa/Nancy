#pragma once
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <iostream>
#include <stdexcept>

namespace fnet {


// socket工具
struct utility {

    // 设置socket为非阻塞
    static int set_nonblocking(int fd) {
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        fcntl(fd, F_SETFL, new_option);
        return old_option;
    }

    /**
     * @brief 快速复用端口
     * @note 失败时抛出异常
     */
    static void set_reuse_address(int fd) {
        int option = 1;
        socklen_t optlen = sizeof(option);
        if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&option, optlen)) {
            throw std::runtime_error(strerror(errno));
        }
    }

    // 禁止Nagle算法
    static void set_tcp_nondelay(int sock) {
        int option;
        if (-1 == setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)(&option), sizeof(option))) {
            throw std::runtime_error(strerror(errno));
        }
    }

    /**
     * @brief 将client地址转换成字符串
     * @param addr 地址
     * @return char*，如127.0.0.1
     */
    static char* to_str(const in_addr& addr) {
        return inet_ntoa(addr);
    } 

};

}


