#include <fastnet/fastnet.h>
#include <time.h>

int main() {
    fnet::reactor rec;
    fnet::acceptor<fnet::protocol::tcp> acp;
    fnet::utility::set_nonblocking(acp.get_fd());
    acp.do_bind("127.0.0.1", 9090);
    acp.do_listen();

    rec.add_acceptor(std::move(acp), [](int fd){
        write(fd, "hello world\n", 13);
    });

    auto sflow = fnet::sigflow::instance();
    sflow->add_signal(SIGINT, [&rec](){
        std::cout<<"\nReceived SIGINT and exit in 1 sec"<<std::endl;
        sleep(1);
        rec.destroy();  // closed the reactor
    });
    rec.add_sigflow(sflow);
    rec.activate();
}