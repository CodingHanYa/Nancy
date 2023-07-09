#include <fastnet/fastnet.h>

int main() {
    fnet::reactor rec;
    fnet::acceptor<fnet::protocol::tcp> acp;
    fnet::utility::set_reuse_address(acp.get_fd());

    std::cout<<"host: 127.0.0.1\nport: 9090\n";
    acp.do_bind("127.0.0.1", 9090);
    acp.do_listen();

    rec.add_acceptor(std::move(acp), [&rec](int new_fd){
        std::cout<<"Accepted:"<<new_fd<<" ip:"<<
        fnet::utility::to_str(rec.remote().sin_addr)<<" port:"<<rec.remote().sin_port<<'\n';
        fnet::utility::set_nonblocking(new_fd);
        rec.add_socket(new_fd, fnet::event::readable, fnet::pattern::et);
    });
    rec.set_readable_cb([](int fd){
        static char buf[1024];
        auto bytes = read(fd, buf, 1024-1);
        buf[bytes] = '\0';
        std::cout<<"recv: "<<buf;
        bytes = write(fd, buf, bytes);
        std::cout<<"send("<<bytes<<")\n";
    });
    rec.set_disconnect_cb([](int fd){
        std::cout<<"Disconnect "<<fd<<'\n';
        close(fd);
    });
    rec.activate();
}