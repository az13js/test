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
    void bindEpollEventPointer(void* epollEvent);
    void* getEpollEventPointer() const;
    void bindCephalopodConnect(void* connect);
    void* getCephalopodConnect() const;
private:
    int fileDescriptor = -1;
    bool isListenFileDescriptor = false;
    void* epollEvent = nullptr;
    void* cephalopodConnect = nullptr;
};

class EpollServer
{
public:
    virtual void start(const std::string& address, int port) final;
    virtual void newConnectionEstablished(EpollServer* server, FileDescriptorEndpoint* endpoint);
    virtual void dataRecvFinish(EpollServer* server, FileDescriptorEndpoint* endpoint, std::string* dataFull, const std::string& errorMessage);
    virtual void holdEndpointCanWrite(EpollServer* server, FileDescriptorEndpoint* endpoint);
    virtual void closeThisEndpoint(FileDescriptorEndpoint* endpoint);
private:
    int epollFileDescriptor = -1;
    std::map<FileDescriptorEndpoint*, bool> endpoints;
    FileDescriptorEndpoint* acceptNewEntpoint(FileDescriptorEndpoint* endpoint);
    void cleanEndpoints();
};

} // namespace cephalopod_epoll

#endif // _cephalopod_epoll_h_
