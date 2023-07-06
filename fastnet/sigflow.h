#pragma once
#include <sys/signal.h>
#include <sys/socket.h>
#include <cstring>
#include <climits>
#include <iostream>
#include <functional>
#include <vector>
#include <queue>
#include "utility.h"

namespace fnet {

class sigflow {
public:
    using sig_handler_t = std::function<void()>;
private:
    inline static int pipefd[2];
    static const char in = 0;
    static const char out = 1;
    sig_handler_t sig_cbs[127];
private:
    sigflow() {
        if (-1 == socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd)) {
            std::cerr<<"[FastNet-Error]: "<<strerror(errno)<<'\n';
            std::abort();
        } 
        utility::set_nonblocking(pipefd[in]); 
        utility::set_nonblocking(pipefd[out]);
    }
    ~sigflow() {
        close(pipefd[in]);
        close(pipefd[out]);
    }   
    static void set_signal(int sig, void(*sig_handler)(int), int sig_flag) {
        struct sigaction sa;
        memset(&sa, '\0', sizeof(sa));
        sa.sa_handler = sig_handler;
        sa.sa_flags |= sig_flag;  
        sigfillset(&sa.sa_mask);
        if (-1 == sigaction(sig, &sa, NULL)) {
            std::cerr<<"[FastNet-Error]: "<<strerror(errno)<<'\n';
            std::abort();
        }
    }
    static void default_handler(int sig) {
        int save_errno = errno;
        int msg = sig;
        if (-1 == send(pipefd[in], (char*)&msg, 1, 0)) {
            throw std::runtime_error("sigflow's buffer overflowed");
        }
        errno = save_errno;
    }
public:
    /**
     * @brief 单例模式获取sigflow实例
     * @return 指向sigflow的指针
     */
    static sigflow* instance() {
        static sigflow flow;
        return &flow;
    }
    
    /**
     * @brief 添加一种信号并设置其处理回调
     * @param sig 信号
     * @param cb 回调函数: void()
     * @param restart 是否无限触发（默认为true）
     */
    void add_signal(int sig, sig_handler_t cb, int flag = SA_RESTART) {
        sig_cbs[sig] = std::move(cb);
        set_signal(sig, default_handler, flag);
    }
    
    /**
     * @brief 删除一种信号，恢复其默认处理方式
     * @param sig 信号
     */
    void del_signal(int sig) {
        set_signal(sig, SIG_DFL, 0);
    }
    
    /**
     * @brief 处理sigflow缓冲区中的信号
     * @return 已处理的信号数
     */
    int process() {
        int bytes, count = 0;
        char sig_buf[8];
        while ((bytes = read(pipefd[out], &sig_buf, 8)) != -1) {
            for (int i = 0; i < bytes; ++i) {
                sig_cbs[sig_buf[i]](); // execute callback
                count++;
            }
        }
        return count;
    }
    
    /**
     * @brief 非阻塞地清除sigflow缓冲区中的信号
     * @note  不执行回调函数
     */
    void clean() {
        char tmp_buf[128];
        while (read(pipefd[out], tmp_buf, 128) != -1) {}
    }

    /**
     * @brief 获取sigflow输出端口
     * @return fd 
     */
    int  get_out() {
        return pipefd[out];
    }
    
    /**
     * @brief 获取sigflow输入端口
     * @return fd
     */
    int  get_in() {
        return pipefd[in];
    }
};

} // namespace fnet