#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <cstring>

namespace fnet {

class cbtrie
{
    unsigned long cnt = 0;
    std::vector<std::unique_ptr<int[]>> nextpos;
    std::vector<std::function<void(char *, size_t)>> cbs;
public:
    cbtrie() = default;
    cbtrie(const cbtrie&) = delete;
    cbtrie(cbtrie&&) = default;
    ~cbtrie() = default;

    /**
     * @brief insert a string literal into the trie and assign it's callback function
     * @tparam N the length of the string literal inferenced by compiler
     * @param cb callback function
     */
    template <size_t N>
    void insert(const char (&str)[N], std::function<void(char *, size_t)> cb) { 
        size_t p = 0;
        for (size_t i = 0; i < (N-1); i++) {
            for (auto i = nextpos.size(); i <= p; ++i) {
                nextpos.emplace_back(new int[26]);
                memset(nextpos.back().get(), 0, 26*sizeof(int)); // clean
            }
            int c = str[i] - 'a';
            if (c >= 0 && c < 26) {
                if (!nextpos[p][c])
                    nextpos[p][c] = ++cnt; // append the trie
                p = nextpos[p][c];
            } else {
                std::cerr<<"invalid charactor"<<std::endl;
                std::abort();
            }
        }
        if (cbs.size() <= p) {
            cbs.resize(p + 1);
        }
        cbs[p] = cb;
    }

    /**
     * @brief insert a string into the trie and assign it's callback function
     * @param str the target str
     * @param len real length of the str (exclude the '\0')
     * @param cb  callback function
     */
    void insert(const char* str, size_t len, std::function<void(char *, size_t)> cb) { 
        size_t p = 0;
        for (size_t i = 0; i < len; i++) {
            for (auto i = nextpos.size(); i <= p; ++i) {
                nextpos.emplace_back(new int[26]);
                memset(nextpos.back().get(), 0, 26*sizeof(int)); // clean
            }
            int c = str[i] - 'a';
            if (c >= 0 && c < 26) {
                if (!nextpos[p][c])
                    nextpos[p][c] = ++cnt; // append the trie
                p = nextpos[p][c];
            } else {
                std::cerr<<"invalid charactor"<<std::endl;
                std::abort();
            }
        }
        if (cbs.size() <= p) {
            cbs.resize(p + 1);
        }
        cbs[p] = cb;
    }

    /**
     * @brief search a string
     * @param str the target str
     * @param len real length of the str (exclude the '\0')
     * @return The position of the str's callback function. If not found, return 0.
     */
    int search(const char *str, size_t len) { 
        size_t p = 0;
        for (size_t i = 0; i < len; i++) {
            int c = str[i] - 'a';
            if (c >= 0 && c < 26) {
                if (!nextpos[p][c]) return 0;
                p = nextpos[p][c];
            } else {
                std::cerr<<"invalid charactor"<<std::endl;
                std::abort();
            }
        }
        return p;
    }

    /**
     * @brief get the reference of the callback function
     * @param p position of the callback function 
     * @return std::function<void(char*, size_t)> 
     */
    auto get(int p) -> std::function<void(char*, size_t)>& {
        return cbs[p];
    }

    /**
     * @brief get the reference of the callback function
     * @param p position of the callback function 
     * @return std::function<void(char*, size_t)> 
     */
    auto operator [](int p) -> std::function<void(char*, size_t)>& {
        return cbs[p];
    }
};

} // namespace fnet
