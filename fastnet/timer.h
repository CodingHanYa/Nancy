#pragma once
#include <chrono>
#include <cstdint>
#include <set>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace fnet::details {

template <class T>
struct duration {
    typedef std::chrono::duration<uint64_t, T> type;
};
template <>
struct duration<void> {
    typedef std::chrono::duration<uint64_t> type;
};

} // fnet::details

namespace fnet {

/**
 * @brief  最高精度的定时器，注意超时回调是非必须的，可以后续绑定
 * @tparam ratio_t 精度，默认为秒(seconds)，可替换为：std::milli|std::micro|std::nano。
 */
template <typename ratio_t = void>
class timer {
public:
    using clock_t = std::chrono::steady_clock;
    using callback_t = void(*)();
    using duration_t = typename details::duration<ratio_t>::type;
    using timestamp_t = std::chrono::time_point<clock_t, duration_t>;

private:
    callback_t  timeout_cb;      // 超时回调
    timestamp_t timeout_stamp = {};   // 超时时间点

public:
    timer(unsigned timeout)
      : timeout_cb([]{})
      , timeout_stamp(std::chrono::time_point_cast<duration_t>(clock_t::now() + duration_t(timeout))) {
    }
    timer(unsigned timeout, const clock_t::time_point& begin)
      : timeout_cb([]{})
      , timeout_stamp(std::chrono::time_point_cast<duration_t>(begin + duration_t(timeout))) {
    }
    timer(const timestamp_t& timeout_stamp)
      : timeout_cb([]{})
      , timeout_stamp(timeout_stamp) {
    }
    timer(const timer& other) = default;
    timer& operator=(const timer& other) = default;
    ~timer() = default;

public:
    /**
     * @brief 设置超时回调
     * @param cb 回调函数
     */
    void set_timeout_cb(callback_t cb) {
        timeout_cb = cb;
    }
    /**
     * @brief 检查是否超时
     * @return true -> 超时
     */
    bool is_timeout() const {
        return std::chrono::time_point_cast<duration_t>(clock_t::now()) > timeout_stamp;
    }
    /**
     * @brief 检查是否超时
     * @return true -> 超时
     */
    bool is_timeout(timestamp_t now) const {
        return now > timeout_stamp;
    }
    /**
     * @brief 检查是否超时
     * @return true -> 超时
     */
    bool is_timeout(clock_t::time_point now) const {
        return std::chrono::time_point_cast<duration_t>(now) > timeout_stamp;
    }
    bool operator < (const timer<ratio_t>& other) const {
        return this->timeout_stamp < other.timeout_stamp;
    }
    void operator ()() const {
        timeout_cb();
    }
};

/**
 * @brief 线程安全的定时器管理器
 * @tparam ratio_t 时间精度
 */
template <typename ratio_t = void>
class timer_master {
    using container_t = typename std::multiset<timer<ratio_t>>;
private:
    bool closed = false;
    std::mutex lok;
    std::thread thrd;
    std::condition_variable thread_cv;
    container_t timers = {};
public:
    explicit timer_master() = default;
    timer_master(const timer_master&) = delete;
    timer_master(timer_master&&) = delete;
    ~timer_master() { 
        if (thrd.joinable()) {
            {
                std::lock_guard<std::mutex> lock(lok);
                closed = true;
            }
            thread_cv.notify_one();
            thrd.join();
        } 

    }
public:
    /**
     * @brief 绑定一个定时器
     * @param t 定时器
     */
    void attach(const timer<ratio_t>& t) {
        std::lock_guard<std::mutex> lock(lok);
        timers.emplace(t);
    }
    /**
     * @brief 剔除一个定时器
     * @param t 定时器
    **/
    void detach(const timer<ratio_t>& t) {
        std::lock_guard<std::mutex> lock(lok);
        timers.erase(t);
    }
    /**
     * @brief 获取保存的定时器数量
     * @return 数量
     */
    size_t num_timers() {
        std::lock_guard<std::mutex> lock(lok);
        return timers.size();
    }
    /**
     * @brief 清除超时定时器
     * @param exec 是否执行回调
     */
    void clean_timeout_timers() {
        timer<ratio_t> temp(0);  // 瞬间过期
        std::lock_guard<std::mutex> lock(lok);
        auto lb = timers.lower_bound(temp);
        for (auto it = timers.begin(); it != lb; ++it) {
            (*it)();
        }
        timers.erase(timers.begin(), lb);
    }
    /**
     * @brief 启动自动清除超时定时器功能
     * @param check_timeval 异步工作线程每次检查的时间间隔
     * @note  时间单位: 毫秒 
     */
    void launch_async_master(unsigned check_timeval) {
        std::lock_guard<std::mutex> lock(lok);
        if (thrd.joinable()) {
            std::cerr<<"[FastNet Error]: Called launch_async_master() again"<<std::endl;
            std::abort();
        } else {
            thrd = std::thread(&timer_master::loop, this, check_timeval);
        }
    }
private:
    void loop(unsigned tval) {
        while (true) {
            std::unique_lock<std::mutex> locker(lok);
            if (closed) return;
            thread_cv.wait_for(locker, std::chrono::milliseconds(tval));
            // 清除并自动执行回调
            timer<ratio_t> temp(0);  // 瞬间过期
            auto lb = timers.lower_bound(temp);
            for (auto it = timers.begin(); it != lb; ++it) {
                (*it)();
            }
            timers.erase(timers.begin(), lb);
        }
    }
};

}  // fnet
