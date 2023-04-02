#include <iostream>
#include "nancy/net/reactor.h"
using namespace nc;

// ================================================================================
//         reactor编写: 单线程+ET+OneShot+Nonblocking的网络模型
// ================================================================================

int main() {
    net::reactor rec;
    net::tcp_serv_socket serv_sock;

    net::set_nonblocking(serv_sock.get_fd());
    net::set_reuse_address(serv_sock.get_fd());
    serv_sock.listen_req("127.0.0.1", 9090);

    auto conn_cb = [&](int) {
        int sock = 0;
        while (1) {
            sock = serv_sock.accept_req();  // 调用原socket的accept接收连接
            if (sock > 0) {
                net::set_nonblocking(sock);  // 设置为非阻塞
                rec.add_socket(sock, net::event::readable, net::pattern::et_oneshot);
                std::cout << "connect fd: " << sock << std::endl;
            } else {
                break;
            }
        }
    };
    rec.add_socket(serv_sock.get_fd(), net::event::readable, net::pattern::et, conn_cb);

    rec.set_readable_cb([&](int fd) {
        const size_t sz = 1024;
        static char buf[sz];
        int ret = read(fd, buf, sz - 1);
        buf[ret] = '\0';
        std::cout << "receive content: " << buf;
        if (ret == (sz - 1)) {
            rec.reset_event(fd, net::event::readable, net::pattern::et_oneshot);
        } else {
            rec.reset_event(fd, net::event::writable, net::pattern::et_oneshot);
        }
    });

    rec.set_writable_cb([&](int fd) {
        const char* hello = "Hello, I'm Nancy\n";
        const size_t len = strlen(hello);
        size_t ret = write(fd, hello, len);
        if (ret == len) {  // 发送完毕，继续监听读事件
            rec.reset_event(fd, net::event::readable, net::pattern::et_oneshot);
        } else {
            std::cout << "[warn]: The data hasn't been sent completly\n";
            rec.reset_event(fd, net::event::readable, net::pattern::et_oneshot);  // 这里仅做简化处理
        }
    });
    rec.set_disconnect_cb([](int fd) {
        close(fd);
        std::cout << "disconnect " << fd << std::endl;
    });
    std::cout<<"服务端启动..."<<std::endl;
    std::cout<<"IP地址: "<<"127.0.0.1"<<std::endl;
    std::cout<<"端口号: "<<9090<<std::endl;
    rec.activate();
}
