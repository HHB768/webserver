#ifndef __HEAPTIMER_HPP__
#define __HEAPTIMER_HPP__

#include <queue>
#include <unordered_map>
#include <time.h>
#include <algorithm>
#include <arpa/inet.h> 
#include <functional> 
#include <assert.h> 
#include <chrono>
// #include "../log/log.h"

namespace mfwu_webserver {

using TimeoutCallBack = std::function<void()>;
using Clock = std::chrono::high_resolution_clock;
using MS = std::chrono::milliseconds;
using TimeStamp = Clock::time_point;

struct TimerNode {
    int id;
    TimeStamp expires;
    TimeoutCallBack cb;
    bool operator<(const TimerNode& t) {
        return expires < t.expires;
    }
};  // endof struct TimerNode

class HeapTimer {
public:
    HeapTimer() { heap_.reserve(64); }
    ~HeapTimer() { clear(); }

    void adjust(int id, int newExpires);
    void add(int id, int timeOut, const TimeoutCallBack& cb);
    void doWork(int id);
    void clear();
    void tick();
    void pop();
    int GetNextTick();

private:
    void del_(size_t i);
    void siftup_(size_t i);
    bool siftdown_(size_t index, size_t n);
    void SwapNode_(size_t i, size_t j);

    std::vector<TimerNode> heap_;
    std::unordered_map<int, size_t> ref_;

};  // endof class HeapTimer


}  // endof namespace mfwu_webserver

#endif  // __HEAPTIMER_HPP__