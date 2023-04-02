#include "nancy/log/log.h"
using namespace nc;

int main() {
    log::asynclogger::initialize("./logfiles/", "test_log", 1);

    // 构造完毕

    LOG_INFO << "Nancy"<<0.1<<" said " <<"hello world";  //自带'\n'
    LOG_WARN << 20 << " is greater then " << 10;
    LOG_CRIT << 'A' << std::string(" string")  << " was logged!";

}
