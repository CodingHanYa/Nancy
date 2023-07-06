#include <fastnet/fastnet.h>
#include <set>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#define TICK_TVAL 10

std::set<int> alive_fds; 
const char* prefix[5] = { "AG_", "QG_", "ES_", "TG_", "DR_"};

int main(int argn, char** args) {
    if (argn != 3) {
        std::cout<<"Usage: ./server [ip] [port]\n";
        std::abort();
    } 
    const char* ip = args[1];
    int port = atoi(args[2]);

    int widx = 0;
    char mesg[64];
    widx = snprintf(mesg, 64, "%s", "Your name is: [");

    // 打开套接字（接收器）
    fnet::acceptor<fnet::protocol::tcp> acp;
    acp.do_bind("127.0.0.1", 9090);
    acp.do_listen();

    // 创建反应堆并添加套接字
    fnet::reactor rec;
    rec.add_acceptor(std::move(acp), [&rec, &mesg, widx](int fd){
        // 非阻塞IO + epoll的LT触发模式
        fnet::utility::set_nonblocking(fd);
        rec.add_socket(fd, fnet::event::readable, fnet::pattern::lt); 
        alive_fds.emplace(fd); // 用集合保存fd
        std::cout<<"connect: "<<fd<<std::endl;
        // send: You name is: what  
        constexpr int num = sizeof(prefix) / sizeof(char*);
        int wlen = snprintf(mesg+widx, 64-widx, "%s%d]\n",  prefix[fd % num], fd);
        mesg[widx + wlen] = '\0';
        write(fd, mesg, widx + wlen + 1);

    });
    rec.set_disconnect_cb([](int fd){
        close(fd);
        alive_fds.erase(fd);
    });

    // =============
    //     信号
    // =============
    // 通过中断信号主动关闭服务器
    fnet::sigflow* pflow = fnet::sigflow::instance();
    pflow->add_signal(SIGINT, [&rec]{
        for (auto fd: alive_fds) {
            write(fd, "The Server was closed...\n", 26);
        } 
        rec.destroy();
        std::cout<<"\n";
    });
    // 通过定时中断实现单线程定时器
    pflow->add_signal(SIGALRM, []{
        std::cout<<"Online: "<<alive_fds.size()<<std::endl;
        alarm(TICK_TVAL); // 每TIME_SHOT秒发送一次
    });
    // 将信号流交由反应堆统一处理
    rec.add_sigflow(pflow); 

    // =================
    //     业务逻辑
    // =================    
    rec.set_readable_cb([](int fd)
    {
        // prepare the name
        constexpr int num = sizeof(prefix) / sizeof(char*);
        std::string name = "-> ";
        name += prefix[fd % num];
        name += std::to_string(fd);
        name += ": ";

        // read content
        static const int name_sz = 12;
        static char buf[name_sz + 1024 + 1];
        int rbs = read(fd, buf + name_sz, 1024);
        buf[name_sz + rbs] = '\0'; // end
        
        // cat <name> && <content>
        int spaces = name_sz - name.length();
        std::strncpy(buf + spaces, name.c_str(), name.length());
        
        // distribute
        for (auto each: alive_fds) {
            if (each != fd) {
                write(each, buf + spaces, name.length() + rbs + 1);
            } 
        }
    });
    alarm(TICK_TVAL);
    // 启动
    rec.activate(); 
}