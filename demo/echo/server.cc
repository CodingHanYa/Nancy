#include "nancy/net/reactor.h"
using namespace nc;

int main() {
    net::reactor rec;
    net::tcp_serv_socket sock;
    sock.listen_req("127.0.0.1", 9090);  // 监听IP+端口
    rec.add_socket(sock.get_fd(), net::event::readable, net::pattern::lt, [&sock, &rec](int){
        int tmp = sock.accept_req();  
        net::set_nonblocking(tmp);   // 非阻塞+lt实现
        rec.add_socket(tmp, net::event::readable, net::pattern::lt);
    });
    char buf[1024];
    rec.set_readable_cb([&buf](int fd) {
        int rcv = read(fd, buf, 1024);
        int bs = write(fd, buf, rcv);
    });
    rec.activate();  // 激活
}
