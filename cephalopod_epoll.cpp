#include "cephalopod_epoll.h"
#include "cephalopod_black_hole.h"

#include <sys/epoll.h> // epoll_create()
#include <cerrno> // errno
#include <cstring> // strerror
#include <unistd.h> // close

namespace cephalopod_epoll {

FileDescriptorEndpoint::FileDescriptorEndpoint(int fileDescriptor, bool isListenFileDescriptor) {
    this->fileDescriptor = fileDescriptor;
    this->isListenFileDescriptor = isListenFileDescriptor;
}

FileDescriptorEndpoint::~FileDescriptorEndpoint() {
    if (-1 != fileDescriptor) {
        close(fileDescriptor);
        fileDescriptor = -1;
    }
    if (nullptr != epollEvent) {
        delete (epoll_event*)epollEvent;
        epollEvent = nullptr;
    }
    if (nullptr != cephalopodConnect) {
        delete (cephalopod_black_hole::Connect*)cephalopodConnect;
        cephalopodConnect = nullptr;
    }
}

int FileDescriptorEndpoint::getFileDescriptor() const {
    return fileDescriptor;
}

bool FileDescriptorEndpoint::isListen() const {
    return isListenFileDescriptor;
}

void FileDescriptorEndpoint::bindEpollEventPointer(void* epollEvent) {
    this->epollEvent = epollEvent;
}

void FileDescriptorEndpoint::bindCephalopodConnect(void* connect) {
    this->cephalopodConnect = connect;
}

void* FileDescriptorEndpoint::getCephalopodConnect() const {
    return cephalopodConnect;
}

void EpollServer::start(const std::string& address, int port) {
    using std::string;
    using std::to_string;
    using cephalopod_black_hole::Connect;
    auto getUnblockFileDescriptor = [](const std::string& address, int port) {
        cephalopod_black_hole::BlackHole fileDescriptorCreater(address, port, true);
        fileDescriptorCreater.setUnBlock();
        return fileDescriptorCreater.getFileDescriptor();
    };

    FileDescriptorEndpoint listenFileDescriptorEndpoint(getUnblockFileDescriptor(address, port), true);

    epollFileDescriptor = epoll_create(1); // Since Linux 2.6.8, the argument is ignored, but must be greater than zero
    if (epollFileDescriptor == -1) {
        auto errorMessage = string("Call epoll_create() fail, errno:") + to_string(errno) + ", info:" + strerror(errno);
        throw errorMessage;
    }

    epoll_event serverEpollEvent;
    serverEpollEvent.events = EPOLLIN | EPOLLET;
    serverEpollEvent.data.ptr = (void*)&listenFileDescriptorEndpoint;

    if (-1 == epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, listenFileDescriptorEndpoint.getFileDescriptor(), &serverEpollEvent)) {
        auto errorMessage = string("Call epoll_ctl(..EPOLL_CTL_ADD..) fail, errno:") + to_string(errno) + ", info:" + strerror(errno);
        close(epollFileDescriptor);
        throw errorMessage;
    }

    int epollEventMax = 1024;
    auto epollEvents = new epoll_event[epollEventMax];
    int readyEventNumber = 0;
    int epollWaitTimeoutMs = 1000;

    while (true) {
        readyEventNumber = epoll_wait(epollFileDescriptor, epollEvents, epollEventMax, epollWaitTimeoutMs);

        if (readyEventNumber < 0) { // When an error occurs, epoll_wait() returns -1
            auto errorMessage = string("Call epoll_wait() fail, errno:") + to_string(errno) + ", info:" + strerror(errno);
            cleanEndpoints();
            close(epollFileDescriptor);
            delete[] epollEvents;
            throw errorMessage;
        }

        for (int idx = 0; idx < readyEventNumber; idx++) {
            auto epollEvent = &(epollEvents[idx]);
            auto endpoint = (FileDescriptorEndpoint*)(epollEvents->data.ptr);
            if (endpoint->isListen()) {
                if (epollEvent->events & EPOLLIN != 0) {
                    FileDescriptorEndpoint* newEndpoint = nullptr;
                    try {
                        newEndpoint = acceptNewEntpoint(endpoint);
                    } catch (const string& e) {
                        cleanEndpoints();
                        close(epollFileDescriptor);
                        delete[] epollEvents;
                        throw e;
                    }

                    auto clientEpollEvent = new epoll_event();
                    clientEpollEvent->events = EPOLLIN | EPOLLOUT | EPOLLET;
                    clientEpollEvent->data.ptr = (void*)newEndpoint;

                    newEndpoint->bindEpollEventPointer((void*)newEndpoint);

                    auto connect = new Connect();
                    connect->open(newEndpoint->getFileDescriptor());
                    newEndpoint->bindCephalopodConnect((void*)connect);

                    if (-1 == epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, newEndpoint->getFileDescriptor(), clientEpollEvent)) {
                        auto errorMessage = string("Call epoll_wait() fail, errno:") + to_string(errno) + ", info:" + strerror(errno);
                        cleanEndpoints();
                        close(epollFileDescriptor);
                        delete[] epollEvents;
                        throw errorMessage;
                    }
                    newConnectionEstablished(this, newEndpoint);
                }
            } else {
                auto connect = (Connect*)(endpoint->getCephalopodConnect());
                if (epollEvent->events & EPOLLIN != 0) {
                    string data;
                    string dataFull;
                    try {
                        while (true) {
                            data.clear();
                            connect->recv(data);
                            dataFull += data;
                        }
                    } catch (const string& e) {
                        dataRecvFinish(this, endpoint, dataFull, e);
                    }
                }
                if (epollEvent->events & EPOLLOUT != 0) {
                    holdEndpointCanWrite(this, endpoint);
                }
            }
        }
        // readyEventNumber is zero if no file descriptor became ready during the requested timeout milliseconds
    }

}

void EpollServer::newConnectionEstablished(EpollServer* server, FileDescriptorEndpoint* endpoint) {
}

void EpollServer::dataRecvFinish(EpollServer* server, FileDescriptorEndpoint* endpoint, const std::string& dataFull, const std::string& errorMessage) {
}

void EpollServer::holdEndpointCanWrite(EpollServer* server, FileDescriptorEndpoint* endpoint) {
}

void EpollServer::closeThisEndpoint(FileDescriptorEndpoint* endpoint) {
    auto it = endpoints.find(endpoint);
    if (it == endpoints.end()) {
        return;
    }
    endpoints.ease(endpoint);
    delete endpoint;
}

FileDescriptorEndpoint* EpollServer::acceptNewEntpoint(FileDescriptorEndpoint* endpoint) {
    cephalopod_black_hole::SocketAccept fileDescriptorCreater(endpoint->getFileDescriptor());
    fileDescriptorCreater.setUnBlock();
    auto newEndpoint = new FileDescriptorEndpoint(fileDescriptorCreater.getFileDescriptor(), false);
    endpoints[newEndpoint] = true;
    return newEndpoint;
}

void EpollServer::cleanEndpoints() {
    for (auto it = endpoints.begin(); it != endpoints.end(); it++) {
        delete (it->first);
    }
    endpoints.clear();
}

} // namespace cephalopod_epoll
