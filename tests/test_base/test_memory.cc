#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include <cassert>
#include "nancy/base/memorys.h"

using namespace nc::base;

void test_memory_unit() {
    memory_unit m1(new char[100], 100);
    memory_unit m2(new char[120], 120);
    m1.swap(m2);
    std::cout<<"m1.size() = "<<m1.size()<<std::endl;
    std::cout<<"m2.size() = "<<m2.size()<<std::endl;
    m1 = std::move(m2);
    std::cout<<"m1.size() = "<<m1.size()<<std::endl;
    std::cout<<"m2.size() = "<<m2.size()<<std::endl;
}

void test_memorys() {
    memorys ms = {
        {2, 4},  // 2 * 4b
        {2, 8}   // 2 * 8b
    };
    assert(!ms.get(12).good()); // 超过最大缓冲

    assert( ms.get(7).good());  // 获取并丢弃 
    assert( ms.get(7).good());  // 获取并丢弃
    assert(!ms.get(7).good());  // 耗尽4b以上的内存单元

    auto unit = ms.get(4);      // 获取内存
    memcpy(unit.get(), "^_^\0", 4);
    std::cout<<(char*)unit.get()<<std::endl;  // void* -> char*

    assert(!ms.count_unit(8));     
    assert(ms.count_unit(4) == 1);   
    assert(ms.count_bytes() == 4); 

    ms.recycle(std::move(unit));   // 回收利用，非常重要
    assert(ms.count_unit(4) == 2); 
    assert(ms.count_bytes() == 8);  

    ms.add(new char[12], 12); // 添加新内存
    ms.add(new char[12], 12); // 添加新内存

    assert(ms.count_unit(12) == 2);
    assert(ms.count_bytes() == (12*2 + 8));

    auto ms2 = std::move(ms);  // 移动构造

    assert(ms.count_unit(12) == 0);
    assert(ms.count_bytes() == 0);

    assert(ms2.count_unit(12) == 2);
    assert(ms2.count_bytes() == (12*2 + 8));
}

void test_simple_memorys() {
    simple_memorys ms(2, 120);  // 10 * 120b
    
    auto unit = ms.get(140);  // invalid demand
    assert(!unit.good() && !unit.size());

    assert(ms.get(110).good());
    assert(ms.count_unit() == 1);
    assert(ms.count_bytes() == 120);

    ms.get(120); // throw
    unit = ms.get(120); // valid demand but out of memory
    assert(!unit.good() && unit.size() == 120);

    ms.add(1); // 只允许指定数量
    assert(ms.count_unit() == 1);
    assert(ms.count_bytes() == 120);

    unit = ms.get(120);
    memcpy(unit.get(), "End of test", 12);
    std::cout<<static_cast<char*>(unit.get())<<std::endl;
}



int main() {
    test_memory_unit();
    test_memorys();
    test_simple_memorys();
}