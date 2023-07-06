#include <fastnet/sockbuffer.h>
#include <fastnet/fastnet.h>
#include <cstring>

fnet::sockbuffer conn;

int main() {
    char mesg[] = "POST / HTTP/1.1\r\n"
                  "Host: www.example.com\r\n"
                  "Content-Type: application/x-www-form-urlencoded\r\n"
                  "Content-Length: 13\r\n"
                  "\r\n"
                  "name=Jenny&age=23";
    std::unique_ptr<char[]> buf(new char[sizeof(mesg)]);
    std::strncpy(buf.get(), mesg, sizeof(mesg));
    conn.replace(std::move(buf), sizeof(mesg));
    std::cout<<conn.readline("\r\n", 2)<<std::endl;
    std::cout<<conn.readline("\r\n", 2)<<std::endl;
    std::cout<<conn.readline("\r\n", 2)<<std::endl;
    std::cout<<conn.readline("\r\n", 2)<<std::endl;
    std::cout<<conn.readline("\r\n", 2).size()<<std::endl;
    std::cout<<conn.readtext(conn.freespace())<<std::endl;

}