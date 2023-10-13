#ifndef _cephalopod_black_hole_h_
#define _cephalopod_black_hole_h_

#include <netinet/in.h>
#include <functional>
#include <string>
#include <vector>
#include <random>
#include <mutex>

namespace cephalopod_black_hole {

bool isIPv4(const std::string& str);
bool isIPv6(const std::string& str);

void checkFileDescriptor(const int fileDescriptor);

class Connect {
public:
    Connect();
    ~Connect();
    void open(const std::string& host, int port);
    void open(const std::string& host, int port, bool withDNSCache);
    void open(int clientFileDescriptor);
    void send(const std::string& data);
    long originSend(int fileDescriptor, const char* buffer, unsigned long bufferSize);
    void recv(std::string& data);
    void close();
    bool isOpen();
private:
    int fileDescriptor = -1;
    std::mutex mt;
    sockaddr_in addressInfo;
#ifndef CEPHALOPOD_NO_IPV6_SUPPORT
    sockaddr_in6 addressInfo6; // for IPv6
#endif
    std::default_random_engine eng;
};

class BlackHole {

public:
    BlackHole(const std::string& address, int port);
    BlackHole(const std::string& address, int port, bool justReturn);
    BlackHole(const std::string& address, int port, const std::function<void(Connect&)>& userCallback);
    BlackHole(const std::string& address, int port, const std::function<void(Connect&)>& userCallback, bool justReturn);
    int getFileDescriptor() const;
    void setUnBlock();
private:
    int listenPort;
    int listenFileDescriptor = -1;
    sockaddr_in bindAddressInfo;
#ifndef CEPHALOPOD_NO_IPV6_SUPPORT
    sockaddr_in6 bindAddressInfo6; // for IPv6
#endif
    void init(const std::string& address, int port, const std::function<void(Connect&)>& userCallback, bool justReturn);
    void bindAddressAndListen(const std::string& address);
    void initBindAddress(const std::string& address);
    void checkBindResult(int bindResult) const;
    void listenSocket();
    void doAcceptAndLoop(const std::function<void(Connect&)>& userCallback);
    void closeFileDescriptor();
};

class SocketAccept {

public:
    SocketAccept(int fileDescriptor);
    void run(const std::function<void(Connect&)>& userCallback);
    void setUnBlock();
    int getFileDescriptor() const;

private:
    socklen_t socketAddressLength;
    sockaddr clientSocketInfo;
    int clientFileDescriptor = -1;
};

} // cephalopod_black_hole

#endif // _cephalopod_black_hole_h_
