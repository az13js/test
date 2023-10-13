#include "cephalopod_black_hole.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <arpa/inet.h>
#include <cstdlib>
#include <unistd.h>
#include <thread>
#include "cephalopod_host_to_ip.h"

//#include "logic.h"

namespace cephalopod_black_hole {

bool isIPv4(const std::string& str) {
    in_addr ipv4Address;
    if (0 == inet_aton(str.c_str(), &ipv4Address)) {
        return false;
    }
    return true;
}

bool isIPv6(const std::string& str) {
#ifdef CEPHALOPOD_NO_IPV6_SUPPORT
    return false;
#else
    in6_addr ipv6Address;
    if (0 == inet_pton(AF_INET6, str.c_str(), &ipv6Address)) {
        return false;
    }
    return true;
#endif
}

void checkFileDescriptor(const int fileDescriptor) {
    using namespace std;
    if (-1 == fileDescriptor) {
        throw string("File descriptor exception, got -1, errno is ") + to_string(errno)
            + ", message: " + string(strerror(errno));
    }
}

Connect::Connect() {
    std::random_device randomSeed;
    eng.seed(randomSeed());
}

Connect::~Connect() {
    close();
}

void Connect::open(const std::string& host, int port) {
    open(host, port, false);
}

void Connect::open(const std::string& host, int port, bool withDNSCache) {
    std::unique_lock<std::mutex> lock(mt);
    if (fileDescriptor >= 0) {
        throw std::string("Unable to open multiple connections");
    }
    auto doConnect = [this](const std::string& host, int port, bool isipv4, bool isipv6) {
        auto size = sizeof(sockaddr_in);
        auto p = (sockaddr *)&addressInfo;
        if (isipv4) {
            addressInfo.sin_family = AF_INET;
            addressInfo.sin_port = htons(port);
            inet_aton(host.c_str(), &(addressInfo.sin_addr));
            fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
        }
#ifndef CEPHALOPOD_NO_IPV6_SUPPORT
        if (isipv6) {
            addressInfo6.sin6_family = AF_INET6;
            addressInfo6.sin6_port = htons(port);
            inet_pton(AF_INET6, host.c_str(), &(addressInfo6.sin6_addr));
            fileDescriptor = socket(AF_INET6, SOCK_STREAM, 0);
            p = (sockaddr *)&addressInfo6;
            size = sizeof(sockaddr_in6);
        }
#endif
        auto tmpFd = fileDescriptor;
        fileDescriptor = -1;
        checkFileDescriptor(tmpFd);
        if (0 != connect(tmpFd, p, size)) {
            auto message = std::string("Connect failed, errno is ") + std::to_string(errno)
                + ", info: " + std::string(strerror(errno));
            ::close(tmpFd);
            throw message;
        }
        // setsockopt
        int keepalive = 1;
        if (0 != setsockopt(tmpFd, SOL_SOCKET, SO_KEEPALIVE, (void*)(&keepalive), sizeof(keepalive))) {
            auto message = std::string("Call setsockopt() failed, errno is ") + std::to_string(errno)
                + ", info: " + std::string(strerror(errno));
            ::close(tmpFd);
            throw message;
        }

        fileDescriptor = tmpFd;
    };

    auto isipv4 = isIPv4(host);
    auto isipv6 = isIPv6(host);

    if (!isipv4 && !isipv6) {
        std::vector<std::string> ipList;
        if (withDNSCache) {
            ipList = cephalopod_host_to_ip::hostToIPsCache(host);
        } else {
            ipList = cephalopod_host_to_ip::hostToIPs(host);
        }
        if (1 == ipList.size()) {
            isipv4 = isIPv4(ipList[0]);
            isipv6 = !isipv4;
            doConnect(ipList[0], port, isipv4, isipv6);
            return;
        }
        std::uniform_int_distribution<int> select(0, ipList.size() - 1);
        auto idx = select(eng);
        isipv4 = isIPv4(ipList[idx]);
        isipv6 = !isipv4;
        doConnect(ipList[idx], port, isipv4, isipv6);
        return;
    }
    doConnect(host, port, isipv4, isipv6);
}

void Connect::open(int clientFileDescriptor) {
    std::unique_lock<std::mutex> lock(mt);
    if (fileDescriptor >= 0) {
        throw std::string("Unable to open multiple connections");
    }
    fileDescriptor = clientFileDescriptor;
}

void Connect::send(const std::string& data) {
    auto sendLength = ::send(fileDescriptor, data.c_str(), data.length(), MSG_NOSIGNAL);
    if (sendLength <= 0) {
        throw std::string("send() < 0") + std::to_string(errno)
            + ", message: " + std::string(strerror(errno));
    }
    while (sendLength > 0 && sendLength < data.length()) {
        auto tmpSendLength = ::send(fileDescriptor, data.c_str() + sendLength, data.length() - sendLength, MSG_NOSIGNAL);
        if (tmpSendLength <= 0) {
            throw std::string("send() <= 0, ") + std::to_string(errno)
                + ", message: " + std::string(strerror(errno));
        }
        sendLength += tmpSendLength;
    }
}

void Connect::recv(std::string& data) {
    char buffer[64];
    int recvDataLength = ::recv(fileDescriptor, buffer, 64, 0);
    if (recvDataLength <= 0) {
        throw std::string("recv() <= 0, ") + std::to_string(errno)
            + ", message: " + std::string(strerror(errno));
    }
    data.assign(buffer, recvDataLength);
}

void Connect::close() {
    std::unique_lock<std::mutex> lock(mt);
    if (fileDescriptor < 0) {
        return;
    }
    shutdown(fileDescriptor, SHUT_RDWR);
    char buffer[1024];
    while (::recv(fileDescriptor, buffer, 1024, 0) > 0) {}
    ::close(fileDescriptor);
    fileDescriptor = -1;
}

bool Connect::isOpen() {
    std::unique_lock<std::mutex> lock(mt);
    return fileDescriptor >= 0;
}

BlackHole::BlackHole(const std::string& address, int port) {
    BlackHole(address, port, [](Connect& connect) {});
}

BlackHole::BlackHole(const std::string& address, int port, const std::function<void(Connect&)>& userCallback) {
    using namespace std;
    listenPort = port;
    auto isipv4 = isIPv4(address);
    auto isipv6 = isIPv6(address);
    if (!isipv4 && !isipv6) {
#ifdef CEPHALOPOD_NO_IPV6_SUPPORT
        throw string("Fail, address must be IPv4");
#else
        throw string("Fail, address must be IPv4 or IPv6");
#endif
    }

    if (isipv4) {
        listenFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    } else {
#ifndef CEPHALOPOD_NO_IPV6_SUPPORT
        listenFileDescriptor = socket(AF_INET6, SOCK_STREAM, 0);
#endif
    }
    checkFileDescriptor(listenFileDescriptor);
    try {
        bindAddressAndListen(address);
        doAcceptAndLoop(userCallback);
    } catch (const string &e) {
        closeFileDescriptor();
        throw e;
    }
}

void BlackHole::bindAddressAndListen(const std::string& address) {
    using namespace std;
    initBindAddress(address);
    int bindResult;
    if (isIPv4(address)) {
        bindResult = bind(listenFileDescriptor, (sockaddr*)&bindAddressInfo, sizeof(sockaddr));
    } else {
#ifndef CEPHALOPOD_NO_IPV6_SUPPORT
        bindResult = bind(listenFileDescriptor, (sockaddr*)&bindAddressInfo6, sizeof(bindAddressInfo6));
#endif
    }
    checkBindResult(bindResult);
    listenSocket();
}

void BlackHole::initBindAddress(const std::string& address) {
    using namespace std;
    if (isIPv4(address)) {
        bindAddressInfo = sockaddr_in();
        bindAddressInfo.sin_family = AF_INET;
        bindAddressInfo.sin_port = htons(listenPort);
        inet_aton(address.c_str(), &(bindAddressInfo.sin_addr));
    } else {
#ifndef CEPHALOPOD_NO_IPV6_SUPPORT
        bindAddressInfo6 = sockaddr_in6();
        bindAddressInfo6.sin6_family = AF_INET6;
        bindAddressInfo6.sin6_port = htons(listenPort);
        inet_pton(AF_INET6, address.c_str(), &(bindAddressInfo6.sin6_addr));
#endif
    }
}

void BlackHole::checkBindResult(int bindResult) const {
    using namespace std;
    if (0 != bindResult) {
        throw string("Binding failed, errno is ") + to_string(errno)
            + ", message: " + string(strerror(errno));
    }
}

void BlackHole::listenSocket() {
    using namespace std;
    auto listenResult = listen(listenFileDescriptor, SOMAXCONN);
    if (0 != listenResult) {
        throw string("Can not listen, errno is ") + to_string(errno)
            + ", message: " + string(strerror(errno));
    }
}

void BlackHole::doAcceptAndLoop(const std::function<void(Connect&)>& userCallback) {
    try {
        while (true) {
            std::thread([&userCallback](SocketAccept* socketAccept) {
                socketAccept->run(userCallback);
                delete socketAccept;
            }, new SocketAccept(listenFileDescriptor)).detach();
        }
    } catch (const std::string& e) {
        closeFileDescriptor();
        throw e;
    } catch (const std::system_error e) {
        closeFileDescriptor();
        throw std::string("Create thread fail");
    }
}

void BlackHole::closeFileDescriptor() {
    if (-1 != listenFileDescriptor) {
        close(listenFileDescriptor);
        listenFileDescriptor = -1;
    }
}

SocketAccept::SocketAccept(int fileDescriptor) {
    socketAddressLength = socklen_t(sizeof(sockaddr));
    clientFileDescriptor = accept(fileDescriptor, &clientSocketInfo, &socketAddressLength);
    checkFileDescriptor(clientFileDescriptor);
    // setsockopt
    int keepalive = 1;
    if (0 != setsockopt(clientFileDescriptor, SOL_SOCKET, SO_KEEPALIVE, (void*)(&keepalive), sizeof(keepalive))) {
        auto errorMessage = std::string("Call setsockopt() failed, errno is ") + std::to_string(errno)
            + ", info: " + std::string(strerror(errno));
        close(clientFileDescriptor);
        clientFileDescriptor = -1;
        throw errorMessage;
    }
}

void SocketAccept::run(const std::function<void(Connect&)>& userCallback) {
    Connect connect;
    connect.open(clientFileDescriptor);
    userCallback(connect);
}

} // cephalopod_black_hole
