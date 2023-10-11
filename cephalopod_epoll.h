#ifndef _cephalopod_epoll_h_
#define _cephalopod_epoll_h_

#include <string>

namespace cephalopod_epoll {

class EpollServer
{
public:
    virtual void start(const std::string& address, int port);
};

} // namespace cephalopod_epoll

#endif // _cephalopod_epoll_h_
