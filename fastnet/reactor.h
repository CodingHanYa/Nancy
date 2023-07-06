#pragma once
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <cassert>
#include <memory>
#include <functional>
#include <unordered_map>
#include "acceptor.h"
#include "utility.h"
#include "sigflow.h"

namespace fnet {  

// epoll event
using event_t = decltype(epoll_event::events);
namespace event {
    static const event_t null = 0;
    static const event_t readable = EPOLLIN; 
    static const event_t writable = EPOLLOUT;
    static const event_t disconnect = EPOLLRDHUP|EPOLLHUP|EPOLLERR;
};

// socket's react patterns
using pattern_t = decltype(epoll_event::events);
namespace pattern {
    static const pattern_t lt = 0;
    static const pattern_t et = EPOLLET;
    static const pattern_t lt_oneshot = EPOLLONESHOT;
    static const pattern_t et_oneshot = EPOLLET | EPOLLONESHOT;
};

/**
 * @brief 可定制不同触发模式和设置事件回调的反应堆
 * @note  配置文件在 事实上所做的配置并不需要多做更改
*/
class reactor {

    bool closed = false;
    int  epoll_fd = 0;
    int  timeout = -1;

    using event_cb_t = std::function<void()>;
    using socket_cb_t = std::function<void(int)>;

    event_cb_t  timeout_cb = {};
    socket_cb_t readable_cb = {};
    socket_cb_t writable_cb = {};
    socket_cb_t dconnect_cb = {};
    std::unordered_map<int, event_cb_t> specific_fds;
    std::unique_ptr<epoll_event[]> ev_buf;

    struct sockaddr_in remote_addr;
    socklen_t remote_addr_sz = sizeof(sockaddr_in);
    static const int ev_buf_sz = 1024;

public:
    explicit reactor()
        : ev_buf(new epoll_event[ev_buf_sz]) {
        epoll_fd = epoll_create(30);
        if (epoll_fd == -1) {
            std::cerr<<"[FastNet-Error]: "<<strerror(errno)<<'\n';
            std::abort();
        }
        timeout_cb  = []{};
        readable_cb = [](int){};
        writable_cb = [](int){};
        dconnect_cb = [](int fd){ close(fd); };
    }
    reactor(const reactor&) = delete;
    reactor(reactor&&) = delete;
    ~reactor() noexcept { destroy(); }

private:
    
    void epoll_add(int sock, event_t ev, pattern_t pattern) {
        struct epoll_event event;
        event.data.fd = sock;
        event.events = ev | pattern | event::disconnect;
        if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &event)) {
            throw std::runtime_error(strerror(errno));
        }
    }
    
    void epoll_mod(int sock, event_t ev, pattern_t pattern) {
        struct epoll_event event;
        event.data.fd = sock;
        event.events = ev | pattern | event::disconnect;
        if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sock, &event)) {
            throw std::runtime_error(strerror(errno));
        }
    }

public:

    /**
     * @brief 添加文件描述符到反应堆中
     * @param fd 文件描述符
     * @param ev 事件（可读/可写/连接断开）
     * @param pattern 事件模式（Epoll的触发模式）
     */
    void add_socket(int fd, event_t ev, pattern_t pattern) {
        epoll_add(fd, ev, pattern);
    }

    /**
     * @brief 添加接收器，可重复添加。reactor不会自动打开接收器进行监听。
     * @param acp 接收器（tcp协议与udp协议皆可）
     * @param connected_cb 回调函数，类型：void(int fd)，传入已接收的fd
     */
    void add_acceptor(acceptor<protocol::tcp>&& acp, socket_cb_t connected_cb) {
        auto acp_fd = acp.release();
        utility::set_nonblocking(acp_fd); 
        epoll_add(acp_fd, event::readable, pattern::et);
        specific_fds.emplace(acp_fd, [=](){
            while (true) {
                int fd = accept(acp_fd, (struct sockaddr*)&remote_addr, &remote_addr_sz);
                if (fd == -1) break;
                else connected_cb(fd);
            }
        });
    }

    /**
     * @brief 统一监听信号流
     * @param flow 信号流 
     */
    void add_sigflow(sigflow* flow) {
        assert(flow != nullptr);
        epoll_add(flow->get_out(), event::readable, pattern::et);
        specific_fds.emplace(flow->get_out(), [=]{
            flow->process();
        });
    }

    /**
     * @brief 更新套接字状态（在oneshot模式下使用）
     * @param fd 套接字
     * @param event 事件类型
     * @param pattern 触发模式
     */
    void reset_event(int fd, event_t event, pattern_t pattern) {
        epoll_mod(fd, event, pattern);
    }
    
    /**
     * @brief 设置epoll等待事件的超时机制
     * @param timeout 超时时长，单位: ms
     * @param cb 超时回调，类型: void() 
     */
    void set_timeout(int timeout, event_cb_t cb) {
        this->timeout = timeout;
        this->timeout_cb = cb;
    }

    /**
     * @brief 设置可读事件回调
     * @param cb 回调函数，类型: void(int fd)
     */
    void set_readable_cb(socket_cb_t cb) {
        readable_cb = std::move(cb);
    }

    /**
     * @brief 设置可写事件回调
     * @param cb 回调函数，类型: void(int fd) 
     */
    void set_writable_cb(socket_cb_t cb) {
        writable_cb = std::move(cb);
    }
    
    /**
     * @brief 设置连接断开（对端关闭或异常）事件回调
     * @param cb 回调函数，类型: void(int fd)
     */
    void set_disconnect_cb(socket_cb_t cb) {
        dconnect_cb = std::move(cb);
    }

    /**
     * @brief 获取最新客户端连接的地址信息等
     * @return sockaddr_in 
     */
    const struct sockaddr_in& remote() {
        return remote_addr;
    }

    /**
     * @brief 激活reactor，并阻塞所在线程
     * @note 想要关闭阻塞的reactor，最好的实践是在事件回调中关闭
     */
    void activate() {
        int fd = 0;
        int ev_nums = 0;
        while (!closed) {
            ev_nums = epoll_wait(epoll_fd, ev_buf.get(), ev_buf_sz, timeout);
            if (!ev_nums) {
                timeout_cb();
            } else {
                for (int i = 0; i < ev_nums; i++) {
                    fd = ev_buf[i].data.fd;
                    auto it = specific_fds.find(fd);
                    if (it != specific_fds.end()) {
                        it->second(); 
                    } else if (ev_buf[i].events & event::disconnect) {
                        dconnect_cb(fd);
                    } else if (ev_buf[i].events & event::readable) {
                        readable_cb(fd);
                    } else if (ev_buf[i].events & event::writable) {
                        writable_cb(fd);
                    }
                }
            }
        }
    }

    /**
     * @brief 关闭反应堆
     * @note 如果是同步关闭，则立刻执行，否则可能会延迟执行
     */
    void destroy() noexcept {
        close(epoll_fd);
        epoll_fd = 0;
        closed = true;
    }
};

}  // namespace nc::net
