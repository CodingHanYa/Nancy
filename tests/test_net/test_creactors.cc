#include "nancy/net/creactors.h"
#include <cstdlib>
#include <iostream>
using namespace nc;

static const char* ip = "127.0.0.1";
static const int port = 9090;

int main(int argn, char** args) {
    assert(argn == 2);
    net::tcp_serv_socket sock;
    net::set_reuse_address(sock.get_fd());
    net::set_tcp_nondelay(sock.get_fd());
    net::set_recv_bufsz(sock.get_fd(), 16*1024);
    sock.listen_req(ip, port);

    net::creactors recs;
    recs.bind_serv_socket(std::move(sock));
    recs.init_async_nodes(atoi(args[1]));

    // 根节点需要额外设置
    recs.root()->reset_timeout(10000);
    recs.root()->set_timeout_cb([](){
        std::cout<<"tick: no request"<<std::endl;
    });

    // 统一设置工作节点 
    recs.set_connect_cb([](net::reactor* rec, int fd){
        net::set_nonblocking(fd);
        rec->add_socket(fd, net::event::readable, net::pattern::lt);
    });
    recs.set_readable_cb([](net::reactor*, int fd){
        int bytes = 0;
        char buf[128];
        while ((bytes = recv(fd, buf, 128, 0)) > 0) { // 读取16k
            send(fd, buf, bytes, 0);
        } 
    });
    recs.set_disconnect_cb([](net::reactor*, int fd){
        std::cout<<"creactors disconnect: "<<fd<<std::endl;
        close(fd);
    });

    // 也可以单独设置工作节点的回调且其拥有更高优先级
    if (recs.node_nums() > 0) {
        recs[0]->set_disconnect_cb([](int fd){
            std::cout<<"reactor disconnect: "<<fd<<std::endl;
            close(fd);
        });
    }
    std::cout<<"server started...\n";
    std::cout<<"working node nums: "<<atoi(args[1])<<"\n";
    std::cout<<"addr: "<<ip<<":"<<port<<"\n";
    recs.activate();
}