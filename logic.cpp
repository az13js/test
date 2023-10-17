#include "logic.h"
#include "cephalopod_log.h"
#include <functional>
#include <random>
#include <thread> // for printLog
#include <sstream> // for printLog
#include <iostream> // for printLog

void printLog(const std::string& message) {
    std::stringstream ss;
    ss << "(thread " << std::this_thread::get_id() << ") " << message;
    cephalopod_log::LogUtil::getLogUtil().info(ss.str());
}

void printLog(const std::string&& message) {
    std::stringstream ss;
    ss << "(thread " << std::this_thread::get_id() << ") " << message;
    cephalopod_log::LogUtil::getLogUtil().info(ss.str());
}

void printLog(const char* message) {
    std::stringstream ss;
    ss << "(thread " << std::this_thread::get_id() << ") " << message;
    cephalopod_log::LogUtil::getLogUtil().info(ss.str());
}

std::string globalGetUUID() {
    static std::mutex mt;
    static std::default_random_engine eng;
    static bool engInit = false;

    std::unique_lock<std::mutex> lock(mt);
    static unsigned long long x = 0;

    if (!engInit) {
        std::random_device randomSeed;
        eng.seed(randomSeed());
        engInit = true;
    }

    return std::to_string(std::uniform_int_distribution<unsigned long>(10000000, 99999999)(eng)) + "-" + std::to_string(x++);
}

NetworkConnection::NetworkConnection() {
    uuid = globalGetUUID();
    printLog(std::string("NetworkConnection init! uuid=") + uuid);
}

void NetworkConnection::bind(cephalopod_black_hole::Connect* connect) {
    this->connect = connect;
    printLog(std::string("NetworkConnection bind()! connect=") + std::to_string((unsigned long)connect));
}

void NetworkConnection::doRecv(std::string& data, cephalopod_pipe::PortState& control) {
    if (connect == nullptr) {
        control = cephalopod_pipe::PortState::CLOSE;
        printLog("NetworkConnection::doRecv, but connect pointer is null! CLOSE!");
        return;
    }
    try {
        connect->recv(data);
        printLog(std::string("NetworkConnection::doRecv ok, connect=") + std::to_string((unsigned long)connect) + std::string(" data size=") + std::to_string(data.length()));
    } catch (const std::string& e) {
        control = cephalopod_pipe::PortState::CLOSE;
        printLog(std::string("NetworkConnection::doRecv fail, connect=") + std::to_string((unsigned long)connect) + std::string(" CLOSE, error:") + e);
    }
}

void NetworkConnection::doSend(const std::string& data, cephalopod_pipe::PortState& control) {
    if (connect == nullptr) {
        control = cephalopod_pipe::PortState::CLOSE;
        printLog("NetworkConnection::doSend, but connect pointer is null! CLOSE!");
        return;
    }
    try {
        connect->send(data);
        printLog(std::string("NetworkConnection::doSend ok, connect=") + std::to_string((unsigned long)connect) + std::string(" data size=") + std::to_string(data.length()));
    } catch (const std::string& e) {
        control = cephalopod_pipe::PortState::CLOSE;
        printLog(std::string("NetworkConnection::doSend fail, connect=") + std::to_string((unsigned long)connect) + std::string(" CLOSE, error:") + e);
    }
}

void NetworkConnection::close() {
    printLog(std::string("NetworkConnection::close, connect=") + std::to_string((unsigned long)connect));
}

std::string NetworkConnection::getFileName(unsigned long id) {
    return std::string("cache/") + uuid + "-" + std::to_string(id);
}

void TransmissionNetworkConnection::recv(std::string& data, cephalopod_pipe::PortState& control) {
    printLog("TransmissionNetworkConnection::recv()");
    doRecv(data, control);
    if (control == cephalopod_pipe::PortState::CLOSE) {
        printLog("TransmissionNetworkConnection::recv - close()");
        connect->close();
    }
}

void TransmissionNetworkConnection::send(const std::string& data, cephalopod_pipe::PortState& control) {
    printLog("TransmissionNetworkConnection::send()");
    doSend(data, control);
    if (control == cephalopod_pipe::PortState::CLOSE) {
        printLog("TransmissionNetworkConnection::send - close()");
        connect->close();
    }
}

void ProxyLogicNetworkConnection::recv(std::string& data, cephalopod_pipe::PortState& control) {
    printLog("ProxyLogicNetworkConnection::recv - wait for a lock");
    std::unique_lock<std::mutex> lock(mt);
    printLog("ProxyLogicNetworkConnection::recv - wait for a lock success!");
    cv.wait(lock, [this]() {
        if (errorMessageSendback != "") {
            printLog("ProxyLogicNetworkConnection::recv - have error message, wait success");
            return true;
        }
        if (connectSuccessMessage != "") {
            printLog("ProxyLogicNetworkConnection::recv - have connect success message, wait success");
            return true;
        }
        if (connect->isOpen()) {
            printLog("ProxyLogicNetworkConnection::recv - connect ok, wait success");
            return true;
        }
        printLog("ProxyLogicNetworkConnection::recv - wait fail");
        return false;
    });
    if (errorMessageSendback != "") {
        data = "HTTP/1.1 503 Service Unavailable\r\n";
        data += "Content-Type: text/plain; charset=UTF-8\r\n";
        data += "Content-Length: " + std::to_string(errorMessageSendback.length()) + "\r\n";
        data += "\r\n";
        data += errorMessageSendback;
        errorMessageSendback.clear();
        control = cephalopod_pipe::PortState::CLOSE;
        printLog(std::string("ProxyLogicNetworkConnection::recv - error message is recv, state=CLOSE, error=") + errorMessageSendback);
        return;
    }
    if (connectSuccessMessage != "") {
        data = connectSuccessMessage;
        connectSuccessMessage.clear();
        printLog("ProxyLogicNetworkConnection::recv - success message is send");
        return;
    }
    if (connect->isOpen()) {
        lock.unlock();
        printLog("ProxyLogicNetworkConnection::recv - connect is open, doRecv");
        doRecv(data, control);
        return;
    }
    control = cephalopod_pipe::PortState::CLOSE;
    printLog("ProxyLogicNetworkConnection::recv - state=CLOSE");
}

void ProxyLogicNetworkConnection::send(const std::string& data, cephalopod_pipe::PortState& control) {
    printLog("ProxyLogicNetworkConnection::send - wait for a lock");
    std::unique_lock<std::mutex> lock(mt);
    printLog("ProxyLogicNetworkConnection::send - lock!");
    if (connect->isOpen() && httpRequest.isConnect()) {
        cv.notify_one();
        lock.unlock();
        printLog("ProxyLogicNetworkConnection::send - just send");
        doSend(data, control);
        printLog("ProxyLogicNetworkConnection::send - send finish");
        return;
    }

    std::function<void(const cephalopod_http::HTTP&)> httpResolvedCallback = [this](const cephalopod_http::HTTP& httpRequest) {
        this->httpRequest = httpRequest;
        httpResolved = true;
    };

    auto dataLength = data.length();
    for (size_t offset = 0; offset < dataLength; offset++) {
        paser.apply(data.substr(offset, 1), httpResolvedCallback);
        if (httpResolved) {
            printLog("ProxyLogicNetworkConnection::send - httpResolved");
            auto host = httpRequest.getConnectHost();
            auto port = httpRequest.getConnectPort();

            if (httpRequest.isConnect()) {
                try {
                    openWithRetry(host, port, 3);
                } catch (const std::string& errorMessage) {
                    errorMessageSendback = errorMessage;
                    control = cephalopod_pipe::PortState::CLOSE;
                    printLog("ProxyLogicNetworkConnection::send - open fail");
                    cv.notify_one();
                    return;
                }
                connectSuccessMessage = "HTTP/1.1 200 Connection Established\r\n\r\n";
                if (offset >= dataLength - 1) {
                    printLog("ProxyLogicNetworkConnection::send - open success, Connection Established");
                    cv.notify_one();
                    return;
                }
                auto dataRealSend = data.substr(offset);
                printLog("ProxyLogicNetworkConnection::send - open success, Connection Established and send extend data");
                doSend(dataRealSend, control);
                std::string controlInfo("SUCCESS");
                if (control == cephalopod_pipe::PortState::CLOSE) {
                    controlInfo = "CLOSE";
                }
                printLog(std::string("ProxyLogicNetworkConnection::send - open success, Connection Established and send extend data finish, control=") + controlInfo);
                cv.notify_one();
                return;
            }

            httpRequest.removeProxyInfo();
            auto dataRealSend = httpRequest.getRequestContent();
            if (connect->isOpen()) {
                if (host != firstRequestHost || port != firstRequestPort) {
                    control = cephalopod_pipe::PortState::CLOSE;
                    printLog("ProxyLogicNetworkConnection::send - host is change, send fail");
                    cv.notify_one();
                    return;
                }
                printLog("ProxyLogicNetworkConnection::send - doSend");
                doSend(dataRealSend, control);
                if (control == cephalopod_pipe::PortState::CLOSE) {
                    printLog("ProxyLogicNetworkConnection::send - doSend fail");
                    cv.notify_one();
                    return;
                }
                continue;
            }
            try {
                printLog("ProxyLogicNetworkConnection::send - remote not ready, opening");
                openWithRetry(host, port, 3);
            } catch (const std::string& errorMessage) {
                errorMessageSendback = errorMessage;
                control = cephalopod_pipe::PortState::CLOSE;
                printLog(std::string("ProxyLogicNetworkConnection::send - open remote fail, errror=") + errorMessage);
                cv.notify_one();
                return;
            }
            printLog("ProxyLogicNetworkConnection::send - send to remote");
            doSend(dataRealSend, control);
            if (control == cephalopod_pipe::PortState::CLOSE) {
                printLog("ProxyLogicNetworkConnection::send - send to remote fail");
                cv.notify_one();
                return;
            }
            httpResolved = false;
            firstRequestHost = host;
            firstRequestPort = port;
        }
    }
    printLog("ProxyLogicNetworkConnection::send - exit");
    cv.notify_one();
}

void ProxyLogicNetworkConnection::openWithRetry(const std::string& host, int port, int maxAccessCount) {
    std::string exception = "In function ProxyLogicNetworkConnection::openWithRetry, info: maxAccessCount <= 0.";
    for (int count = 0; count < maxAccessCount; count++) {
        try {
            connect->open(host, port, true);
            return;
        } catch (const std::string& errorMessage) {
            exception = errorMessage;
        }
    }
    throw exception;
}
