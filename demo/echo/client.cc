#include <iostream>
#include "nancy/net/socket.h"
using namespace nc;

int main() {
    const char* hello = "Hello Nancy";
    net::tcp_clnt_socket clnt;

    clnt.launch_req("127.0.0.1", 9090);
    int tmp = write(clnt.get_fd(), hello, strlen(hello));
    std::cout<<"block in write"<<std::endl;

    char buf[128];
    int ret = read(clnt.get_fd(), buf, 128);
    buf[ret] = '\0';
    std::cout << "recv: " << buf << std::endl;
}