#pragma once
#include <sys/socket.h>
#include <cstring>
#include <string_view>
#include <memory>


namespace fnet {

class sockbuffer {
    std::unique_ptr<char[]> buf = {};
    size_t buf_sz = 0;
    char* p_rd = nullptr; 
    char* p_wd = nullptr;
    char* p_end = nullptr;
    int fd = 0;
public:
    sockbuffer() = default;
    sockbuffer(int fd, size_t buf_sz)
        : buf(new char[buf_sz])
        , buf_sz(buf_sz)
        , p_rd(buf.get()) 
        , p_wd(buf.get())
        , p_end(buf.get() + buf_sz)
        , fd(fd) {}
    sockbuffer(const sockbuffer&) = delete;
    sockbuffer(sockbuffer&&) = default;
    ~sockbuffer() = default;
    sockbuffer& operator = (sockbuffer&&) = default;
public:
    /**
     * @brief 从socket缓冲中读取内容到内存中
     * @return 返回读取的字节数
     * @note 此调用会快速填充未写的缓冲区
     */
    size_t readsock() {
        size_t count = 0;
        while (1) {
            int b = recv(fd, p_wd, p_end-p_wd, MSG_DONTWAIT);
            if (b <= 0) {
                break;
            } else {
                p_wd += b;
                count += b;
            }
        }
    }

    /**
     * @brief 从内部缓冲中读出新的一行
     * @param end 行分割符（字符串），需以'\0'为
     * @param end_len 行分隔符的长度
     * @return std::string_view 新行的视图，若无法解析出新行，则返回空string_view，并且不消耗待处理字节
     * @note 该接口会通过移动内部指针消耗掉未读的内容
     */
    std::string_view readline(const char* end, size_t end_len) {
        std::string_view view(p_rd, p_wd - p_rd);
        auto pos = view.find(end);
        if (pos == view.npos) {
            return std::string_view();
        } else {
            p_rd[pos] = '\0';
            std::string_view res(p_rd, pos);
            p_rd += (pos + end_len);
            return res;
        }
    }

    /**
     * @brief 从内部缓冲中读出文本
     * @param len 需要读取的文本长度
     * @return 实际读取文本的视图
     * @note 该接口会通过移动内部指针消耗掉未读的内容
     */
    std::string_view readtext(size_t len) {
        size_t l = p_wd - p_rd;
        len = (l > len) ? len : l;
        std::string_view view(p_rd, len);
        p_rd += len;
        return view;
    }

    /**
     * @brief 获取待处理字节数
     * @return size_t 字节数
     */
    size_t pending() {
        return p_wd - p_rd;
    }

    /**
     * @brief 获取空闲可读空间
     * @return size_t 字节数
     */
    size_t freespace() {
        return p_end - p_wd;
    }

    /**
     * @brief 清空内部缓冲区（单纯改变指针指向O(1)）
     * @note 未读的f内容会被丢弃
     */
    void reflush() {
        p_rd = buf.get();
        p_wd = p_rd;
    }

    /**
     * @brief 替换掉缓冲区
     * @param new_buf 新的缓冲区 
     * @param new_buf_sz 新缓冲区的尺寸
     * @return std::unique_ptr<char[]> 旧的缓冲区
     */
    auto replace(std::unique_ptr<char[]> new_buf, size_t new_buf_sz) -> std::unique_ptr<char[]> {
        buf.swap(new_buf);
        buf_sz = new_buf_sz;
        p_end = buf.get() + buf_sz;
        reflush();
        return new_buf;
    }

    /**
     * @brief 替换掉缓冲区并内部生成新缓冲区
     * @param new_buf_sz 新缓冲区的大小
     * @return std::unique_ptr<char[]> 旧的缓冲区
     */
    auto replace(size_t new_buf_sz) -> std::unique_ptr<char[]> {
        auto old = buf.release();
        buf.reset(new char[new_buf_sz]);
        buf_sz = new_buf_sz;
        p_end = buf.get() + buf_sz;
        reflush();
        return std::unique_ptr<char[]>(old);
    }


    /**
     * @brief 丢弃已读的内容
     * @note 未被读的内容会被拷贝到缓冲区的前端，并保持未读的状态
     */
    void drop_read() {
        strncpy(buf.get(), p_rd, p_wd-p_rd);
        p_wd = buf.get() + (p_wd - p_rd);
        p_rd = buf.get();
    }
};


} // namespace fnet