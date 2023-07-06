#include <cassert>
#include <iostream>
#include <functional>
#include <thread>
#include <fastnet/timer.h>
using namespace fnet;

// 测试我们的定时器，确保其能准确反映记录的时间
void test_timer() {
    std::cout << "---------------------------\n";
    timer<> t1(2);               // 2s
    timer<std::milli> t2(2000);  // 2000 mili

    auto now = std::chrono::steady_clock::now();
    assert(!t1.is_timeout(now) && !t2.is_timeout(now));

    std::this_thread::sleep_for(std::chrono::duration<int64_t, std::milli>(999));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 1999ms ~ 1s
    if (!t1.is_timeout()) std::cout << "1999ms timer(2s)     not timeout\n";
    if (!t2.is_timeout()) std::cout << "1999ms timer(2000ms) not timeout\n";

    // 2000ms = 2s
    std::this_thread::sleep_for(std::chrono::duration<int64_t, std::milli>(1));
    if (t1.is_timeout()) std::cout << "2000ms timer(2s)     timeout\n";
    else std::cout << "2000ms timer(2s)     not timeout\n";
    if (t2.is_timeout()) std::cout << "2000ms timer(2000ms) timeout\n";
    else std::cout << "2000ms timer(2000ms) not timeout\n";

    // 2001ms ~ 2s
    std::this_thread::sleep_for(std::chrono::duration<int64_t, std::milli>(1));
    if (t1.is_timeout()) std::cout << "2001ms timer(2s)     timeout\n";
    else std::cout << "2001ms timer(2s)     not timeout\n";
    if (t2.is_timeout()) std::cout << "2001ms timer(2000ms) timeout\n";
    else std::cout << "2001ms timer(2000ms) not timeout\n";

    // 3001ms ~ 3s
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (t1.is_timeout()) std::cout << "3001ms timer(2s)     timeout\n";
    else std::cout << "3001ms timer(2s)     not timeout\n";
    std::cout << "---------------------------\n";
}

// 测试定时器管理器自动清除超时定时器的功能
void test_timer_master() {
    std::cout << "---------------------------\n";

    timer<std::milli> t1(10);
    timer<std::milli> t2(20);
    timer<std::milli> t3(1005);

    // 设置超时回调
    t1.set_timeout_cb([]{std::cout<<"t1 timeout\n";});
    t2.set_timeout_cb([]{std::cout<<"t2 timeout\n";});
    t3.set_timeout_cb([]{std::cout<<"t3 timeout\n";});

    timer_master<std::milli> master;
    master.attach(t1);
    master.attach(t2);
    master.attach(t3);

    master.launch_async_master(500);
    std::cout<<"last: "<<master.num_timers()<<std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));  // 1000 ms
    std::cout<<"last: "<<master.num_timers()<<std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(501));  // 501ms (等到第三次检查超时定时器)
    std::cout<<"last: "<<master.num_timers()<<std::endl;
    std::cout << "---------------------------\n";
}

int main() {
    test_timer();
    test_timer_master();
}