#ifndef __THREADPOOL_HPP__
#define __THREADPOOL_HPP__

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>
#include<assert.h>

// 当 std::condition_variable 对象的某个 wait 函数被调用的时候，
// 它使用 std::unique_lock(通过 std::mutex) 来锁住当前线程。
// 当前线程会一直被阻塞，直到另外一个线程在相同的 
// std::condition_variable 对象上调用了 notification 函数来唤醒当前线程。
// 例：
// #include <iostream>                // std::cout
// #include <thread>                // std::thread
// #include <mutex>                // std::mutex, std::unique_lock
// #include <condition_variable>    // std::condition_variable
 
// std::mutex mtx; // 全局互斥锁.
// std::condition_variable cv; // 全局条件变量.
// bool ready = false; // 全局标志位.
 
// void do_print_id(int id)
// {
//     std::unique_lock <std::mutex> lck(mtx);
//     while (!ready) // 如果标志位不为 true, 则等待...
//         cv.wait(lck); // 当前线程被阻塞, 当全局标志位变为 true 之后,
//     // 线程被唤醒, 继续往下执行打印线程编号id.
//     std::cout << "thread " << id << '\n';
// }
 
// void go()
// {
//     std::unique_lock <std::mutex> lck(mtx);
//     ready = true; // 设置全局标志位为 true.
//     cv.notify_all(); // 唤醒所有线程.
// }
 
// int main()
// {
//     std::thread threads[10];
//     // spawn 10 threads:
//     for (int i = 0; i < 10; ++i)
//         threads[i] = std::thread(do_print_id, i);
 
//     std::cout << "10 threads ready to race...\n";
//     go(); // go!
 
//   for (auto & th:threads)
//         th.join();
 
//     return 0;
// }


namespace mfwu_webserver {

class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount=8) 
        : pool_(std::make_shared<Pool>()) {
        assert(threadCount > 0);
        for (size_t i = 0; i < threadCount; i++) {
            std::thread([pool = pool_] {
                std::unique_lock<std::mutex> locker(pool->mtx);
                while (true) {
                    if (!pool->tasks.empty()) {
                        auto task = std::move(pool->tasks.front());
                        pool->tasks.pop();
                        locker.unlock();
                        task();
                        locker.lock();
                    } else if (pool->isClosed) break;
                    else pool->cond.wait(locker);
                }
            }).detach();
        }
    }

    ThreadPool() = default;
    ThreadPool(ThreadPool&&) = default;

    ~ThreadPool() {
        if (static_cast<bool>(pool_)) {
            {
                std::lock_guard<std::mutex> locker(pool_->mtx);
                pool_->isClosed = true;
            }
            pool_->cond.notify_all();
        }
    }

    template <typename T>
    void AddTask(T&& task) {
        {
            std::lock_guard<std::mutex> locker(pool_->mtx);
            pool_->tasks.emplace(std::forward<T>(task));
        }
        pool_->cond.notify_one();
    }

private:
    struct Pool {
        std::mutex mtx;
        std::condition_variable cond;
        bool isClosed;
        std::queue<std::function<void()>> tasks;
    };  // endof struct Pool

    std::shared_ptr<Pool> pool_;

};  // endof class ThreadPool


}  // endof namespace mfwu_webserver

#endif  // __THREADPOOOL_HPP__