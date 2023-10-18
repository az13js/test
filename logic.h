#ifndef _logic_h_
#define _logic_h_

#include "cephalopod_black_hole.h"
#include "cephalopod_pipe.h"
#include "cephalopod_http.h"
#include <string>
#include <condition_variable>
#include <mutex>

void printLog(const std::string& message);
void printLog(const std::string&& message);
void printLog(const char* message);

std::string globalGetUUID();

class NetworkConnection: public cephalopod_pipe::Port {
public:
    NetworkConnection();
    virtual void bind(cephalopod_black_hole::Connect* connect);
    virtual void doRecv(std::string& data, cephalopod_pipe::PortState& control);
    virtual void doSend(const std::string& data, cephalopod_pipe::PortState& control);
    virtual void close();
    virtual std::string getFileName(unsigned long id);
protected:
    cephalopod_black_hole::Connect* connect = nullptr;
private:
    std::string uuid;
};

class TransmissionNetworkConnection: public NetworkConnection {
public:
    void recv(std::string& data, cephalopod_pipe::PortState& control);
    void send(const std::string& data, cephalopod_pipe::PortState& control);
};

class ProxyLogicNetworkConnection: public NetworkConnection {
public:
    void recv(std::string& data, cephalopod_pipe::PortState& control);
    void send(const std::string& data, cephalopod_pipe::PortState& control);
    void close();
private:
    cephalopod_http::HTTPPaser paser = cephalopod_http::HTTPPaser();
    cephalopod_http::HTTP httpRequest = cephalopod_http::HTTP();
    std::mutex mt;
    std::condition_variable cv;
    bool httpResolved = false;
    std::string errorMessageSendback = "";
    std::string connectSuccessMessage = "";
    std::string firstRequestHost = "";
    int firstRequestPort = -1;
    bool notifyConnectionClose = false;
    void openWithRetry(const std::string& host, int port, int maxAccessCount);
};

#endif // _logic_h_
