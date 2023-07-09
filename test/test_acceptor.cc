#include <fastnet/acceptor.h>
#include <iostream>

int main() {
    fnet::acceptor<fnet::protocol::tcp> acp;
    std::cout<<"host: 127.0.0.1\nport: 9090\n";
    acp.do_bind("127.0.0.1", 9090);
    acp.do_listen(5);

    int fd = 0;
    while ((fd = acp.do_accept())) {
        std::cout<<"connected fd: "<<fd<<std::endl;
        close(fd);
    }

}