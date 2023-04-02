#include <nancy/log/log.h>
using namespace nc;

std::mutex  log::asynclogger::obj_lock;
std::unique_ptr<log::asynclogger> log::asynclogger::logger = {nullptr};

