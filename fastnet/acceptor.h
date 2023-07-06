#pragma once
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "utility.h"

namespace fnet {

// 用于模板特化的类型
struct protocol
{
    struct tcp {}; 
    struct udp {};
};

// 接收器
template <typename Protocol>
class acceptor {};

// tcp acceptor
template <>
class acceptor<protocol::tcp> {
    int  sock = 0;
    struct sockaddr_in remote_info_buf;
    socklen_t remote_addr_sz = sizeof(struct sockaddr_in);

public:
    /**
     * @brief 构造接收器并绑定IP+端口
     * @param ip ip地址
     * @param port 端口号
     * @param reuse_addr 是否快速复用端口 
     */
    explicit acceptor() {
        sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            std::cerr<<"[FastNet-Error]: "<<strerror(errno)<<std::endl;
            std::abort();
        } 
    }
    acceptor(const acceptor&) = delete;
    acceptor(acceptor&& other) noexcept {
        sock = other.release();
    }
    acceptor& operator=(const acceptor&) = delete;
    acceptor& operator=(acceptor&& other) {
        sock = other.release();
        return *this;
    }
    ~acceptor() {
        do_close();
    }

public:
    /**
     * @brief 开始监听
     * @param backlog 最多积压的连接数，达到该数后新连接会被丢弃
     */
    void do_listen(int backlog = 511) {
        if (-1 == listen(sock, backlog)) {
            std::cerr<<"[FastNet-Error]: "<<strerror(errno)<<std::endl;
            std::abort();
        }
    }

    /**
     * @brief 绑定IP与端口，并设置是否快速复用端口
     * @param ip ip地址
     * @param port 端口
     * @param reuse_addr 是否快速复用端口 
     */
    void do_bind(const char* ip, int port) {
        struct sockaddr_in sock_addr;
        std::memset(&sock_addr, 0, sizeof(sock_addr));
        inet_pton(AF_INET, ip, &sock_addr.sin_addr);
        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = htons(port);
        if (-1 == bind(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr))) {
            std::cerr<<"[FastNet-Error]: "<<strerror(errno)<<std::endl;
            std::abort();
        }
    }

    /**
     * @brief 关闭接收器
     * @note 该函数时可重入的，但不建议随意调用
     */
    void do_close() {
        close(sock);
        sock = 0;
    }

    /**
     * @brief 阻塞等待接收一个连接
     * @return 成功: fd ; 失败: -1
     */
    int do_accept() {
        return accept(sock, (struct sockaddr*)&remote_info_buf, &remote_addr_sz);
    }

    /**
     * @brief 获取最新客户端连接的地址信息
     * @return sockaddr_in 
     */
    const struct sockaddr_in& remote_info() {
        return remote_info_buf;
    }

    /**
     * @brief 返回内部fd
     * @return fd
     */
    int get_fd() const noexcept {
        return sock;
    }

    /**
     * @brief 获取该fd并将内部fd置为0，类似智能指针的release
     * @return 内部fd
     */
    int release() noexcept {
        auto res = sock;
        sock = 0;
        return res;
    }
};


} // namespace fnet