#ifndef __EPOLLER_HPP__
#define __EPOLLER_HPP__

#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <errno.h>

namespace mfwu_webserver {

class Epoller {
public:
    explicit Epoller(int maxEvent=1024);
    ~Epoller();

    bool AddFd(int fd, uint32_t events);
    bool ModFd(int fd, uint32_t events);
    bool DelFd(int fd);

    int Wait(int timeoutMs=-1);
    int GetEventFd(size_t i) const;
    uint32_t GetEvents(size_t i) const;

private:
    int epollFd_;
    std::vector<struct epoll_event> events_;
};  // endof class Epoller


}  // endof namespace mfwu_webserver

#endif  // __EPOLLER_HPP__