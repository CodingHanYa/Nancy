#pragma once
#include <fastnet/sockbuffer.h>
#include <fastnet/cbtrie.h>
#include <cstring>
#include <iostream>
#include <string_view>
#include <vector>
#include <cassert>
#include <functional>
namespace fnet::details {

char tokens[] = {};


} // namespace fnet::details

namespace fnet {

class http_parser;
class http_request;
struct http_reqline;

struct http_reqline {
    enum methods {
        GET,
        POST
    } method;
    const char* url;
    size_t url_len;
    const char* version;
    size_t version_len;
};

enum class http_state {
    ON_REQ_LINE = 0,
    ON_REQ_HEAD = 1,
    ON_REQ_BODY = 2,
    ON_REQ_MORE = 3,
    ON_REQ_DONE = 4,
    ON_REQ_ERRO = 5
};

class http_request {
public:
    http_request(int fd, size_t buf_sz): rbuf(fd, buf_sz) {}
    ~http_request() = default;
public:
    bool is_bad() {
        return req_sta == state::req_erro;
    }
    bool is_done() {
        return req_sta == state::req_done;
    }
private:
    enum class state {
        req_done,
        req_erro,
        req_more
    };
    sockbuffer rbuf;
    http_state old_sta;
    http_state new_sta;
    state req_sta;
    friend class http_parser;
};

class http_parser {

public:
    
    using reqfield_cb_t = std::function<void(char*, size_t)>;
    using reqbody_cb_t = std::function<void(char*, size_t)>;
    using reqline_cb_t = std::function<void(http_reqline)>;

    /**
     * @brief 处理来自socket的新数据
     * @note 该接口会维护旧的状态，保证报文解析流程的完整性
     */
    void process(http_request& req) {
        while (1) {
            switch (req.new_sta) 
            {
                case http_state::ON_REQ_LINE: {
                    auto view = req.rbuf.readline("\r\n", 2);
                    if(!view.size()) {
                        break;
                    } 
                }
                case http_state::ON_REQ_HEAD: {
                    break;
                }
                case http_state::ON_REQ_BODY: {
                    break;
                }
                case http_state::ON_REQ_MORE: {
                    if (req.rbuf.freespace() > 0) {
                        auto sz = req.rbuf.readsock();
                        if (!sz) return; 
                    } else {
                        req.rbuf.drop_read(); 
                        auto sz = req.rbuf.readsock();
                        if (!sz) return; 
                    }
                    req.new_sta = req.old_sta; // recover and keep parsing
                    req.old_sta = http_state::ON_REQ_MORE;
                    break;
                }
                case http_state::ON_REQ_DONE: {
                    req.rbuf.drop_read();
                    req.new_sta = http_state::ON_REQ_LINE;
                    return;
                }
                case http_state::ON_REQ_ERRO: {

                }
            }
        }
    }

    void on_reqline_do(reqline_cb_t cb) {
        reqline_cb = cb;
    }
    void on_reqbody_do(reqbody_cb_t cb) {
        reqbody_cb = cb;
    }
    template <size_t N>
    void on_reqfield_do(const char (&field)[N], reqfield_cb_t do_what) {
        reqbody_cbs.insert(field, N-1, do_what);
    }
    
private:
    cbtrie reqbody_cbs;
    reqline_cb_t reqline_cb;
    reqbody_cb_t reqbody_cb;
};



} // namespace fnet