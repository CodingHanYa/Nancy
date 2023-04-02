#include <iostream>
#include <thread>
#include "nancy/base/timer.h"
using namespace nc;

int main() {
    // 微秒级定时器管理器
    base::timer_master<std::milli> master;

    // 定时器
    base::timer<std::milli> t1(10);
    base::timer<std::milli> t2(20);
    base::timer<std::milli> t3(30);
    base::timer<std::milli> t4(40);

    t1.bind([] { std::cout << "t1 called\n"; });
    t2.bind([] { std::cout << "t2 called\n"; });
    t3.bind([] { std::cout << "t3 called\n"; });
    t4.bind([] { std::cout << "t4 called\n"; });

    // 附着到master上
    t1.attach(master);  // or master.bind(t1)
    t2.attach(master);
    t3.attach(master);
    t4.attach(master);

    std::cout << master.size() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(21));
    master.clean_timeout();
    std::cout << master.size() << std::endl;
}