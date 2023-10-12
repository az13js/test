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
}

int FileDescriptorEndpoint::getFileDescriptor() const {
    return fileDescriptor;
}

bool FileDescriptorEndpoint::isListen() const {
    return isListenFileDescriptor;
}

void EpollServer::start(const std::string& address, int port) {
    using std::string;
    using std::to_string;
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
            delete[] epollEvents;
            auto errorMessage = string("Call epoll_wait() fail, errno:") + to_string(errno) + ", info:" + strerror(errno);
            close(epollFileDescriptor);
            throw errorMessage;
        }

        for (int idx = 0; idx < readyEventNumber; idx++) {
            auto epollEvent = &(epollEvents[idx]);
            auto endpoint = (FileDescriptorEndpoint*)(epollEvents->data.ptr);
            if (endpoint->isListen()) {
                
            }
        }
        // readyEventNumber is zero if no file descriptor became ready during the requested timeout milliseconds
    }

}

} // namespace cephalopod_epoll
