#include <cassert>
#include <iostream>
#include <thread>
#include "nancy/base/timer.h"
using namespace nc::base;

// 测试我们的定时器，确保其能准确反映记录的时间
void test_timer_validity() {
    timer<> t1(2);               // 2s
    timer<std::milli> t2(2000);  // 2000 mili

    auto now = std::chrono::steady_clock::now();
    assert(!t1.check(now) && !t2.check(now));

    std::this_thread::sleep_for(std::chrono::duration<int64_t, std::milli>(999));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 1999ms = 1s
    if (!t1.check()) {
        std::cout << "1999 timer(2s) not done\n";
    }
    if (!t2.check()) {
        std::cout << "1999 timer(2000ms) not done\n";
    }
    // 2000ms = 2s
    std::this_thread::sleep_for(std::chrono::duration<int64_t, std::milli>(1));
    if (t1.check()) {
        std::cout << "2000 timer(2s) done\n";
    }
    if (t2.check()) {
        std::cout << "2000 timer(2000ms) done\n";
    }
    // 2001ms = 2s
    std::this_thread::sleep_for(std::chrono::duration<int64_t, std::milli>(1));
    if (t1.check()) {
        std::cout << "2001 timer(2s) done\n";
    }
    if (t2.check()) {
        std::cout << "2001 timer(2000ms) done\n";
    }
    // 3001ms = 3s
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (t1.check()) {
        std::cout << "3001 timer(2s) done\n";
    }
}

// 测试一些其它接口
void test_timer_interfaces() {
    std::cout << "---------------------------\n";

    timer<std::micro> t1(10);
    timer<std::micro> t2(20);
    assert(t1 < t2);

    t1.reset(21);
    assert(t2 < t1);  // t1: 21  t2: 20

    t2 = t1;
    assert(!(t1 < t2));

    t1();  // no exception
    t1.bind([] { std::cout << "hello world\n"; });
    t1();  // invoke
}

int main() {
    test_timer_validity();
    test_timer_interfaces();
}