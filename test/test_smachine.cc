#include <fastnet/smachine.h>
#include <iostream>

enum class http {
    ON_REQUEST,
    ON_HEAD,
    ON_BODY
};

int main() {
    fnet::smachine<http> sm;
    sm.add_state(http::ON_REQUEST, [&sm]{ 
        std::cout<<"Enter\n";
        std::cout<<"state: http-ON_REQUEST"<<std::endl;
        sm[http::ON_HEAD].run();
    });
    sm.add_state(http::ON_HEAD, [&sm]{
        std::cout<<"state: http-ON_HEAD"<<std::endl;
        sm.run(http::ON_BODY);
    });
    sm.add_state(http::ON_BODY, [&sm]{
        std::cout<<"state: http-ON_BODY"<<std::endl;
        std::cout<<"Out\n";
    });
    sm.run(http::ON_REQUEST);
}