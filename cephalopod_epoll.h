#ifndef _cephalopod_epoll_h_
#define _cephalopod_epoll_h_

#include <string>
#include <map>

namespace cephalopod_epoll {

class FileDescriptorEndpoint {
public:
    FileDescriptorEndpoint(int fileDescriptor, bool isListenFileDescriptor);
    ~FileDescriptorEndpoint();
    int getFileDescriptor() const;
    bool isListen() const;
private:
    int fileDescriptor = -1;
    bool isListenFileDescriptor = false;
};

class EpollServer
{
public:
    virtual void start(const std::string& address, int port) final;
private:
    int epollFileDescriptor = -1;
    std::map<FileDescriptorEndpoint*, bool> endpoints;
    FileDescriptorEndpoint* acceptNewEntpoint(FileDescriptorEndpoint* endpoint);
    void cleanEndpoints();
};

} // namespace cephalopod_epoll

#endif // _cephalopod_epoll_h_
