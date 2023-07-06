#pragma once 
#include <functional>
#include <vector>

namespace fnet {

/**
 * @brief  单一状态机，只接收一种enum类型
 * @tparam E 状态enum类型
 * @tparam I 在进行索引时将enum类型转换为什么类型（如int, unsigned, uint8_t等）
*/
template <typename E, 
        typename I = uint16_t, 
        typename = typename std::enable_if<std::is_integral<I>::value>::type>
class smachine {
public:
    using sta_callback_t = std::function<void()>;
private:
    class state_item {
        E sta = {};
        sta_callback_t cb = {};
    public:
        state_item() = default;
        state_item(E sta, sta_callback_t cb)
            : sta(sta)
            , cb(std::move(cb)) {}
        state_item(state_item&&) = default;
        state_item& operator=(state_item&& other) = default;

        void run() { cb(); }
        E get() { return sta; }
    };
    std::vector<state_item> states;
public:
    /**
     * @brief 构造函数
     * @param max_sta 状态最大值
     */
    smachine(int max_sta = 15): states(max_sta) {};
    ~smachine() = default;
public:
    /**
     * @brief 添加状态与处理回调
     * @param sta 状态
     * @param cb 回调
     */
    void add_state(E sta, sta_callback_t cb) {
        if ((I)sta >= states.size()) {
            states.resize((I)sta+1);
        }
        states[(I)sta] = state_item(sta, std::move(cb));
    }
    /**
     * @brief 获取状态项（包含状态与回调）
     * @param sta 状态
     * @return state_item
     */
    state_item& operator[] (E sta) {
        return states[(I)sta];
    }
    /**
     * @brief 执行状态回调
     * @param sta 状态
     */
    void run(E sta) {
        states[(I)sta].run();
    }
};

} // fnet