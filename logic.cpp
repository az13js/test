#include "logic.h"
#include <functional>
#include <random>
#include <iostream>

void printLog(const std::string& message) {
    std::unique_lock<std::mutex> lock(printLogMt);
    std::cout << message << std::endl;
}

void printLog(const std::string&& message) {
    std::unique_lock<std::mutex> lock(printLogMt);
    std::cout << message << std::endl;
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
}

void NetworkConnection::bind(cephalopod_black_hole::Connect* connect) {
    this->connect = connect;
}

void NetworkConnection::recv(std::string& data, cephalopod_pipe::PortState& control) {
    doRecv(data, control);
}

void NetworkConnection::doRecv(std::string& data, cephalopod_pipe::PortState& control) {
    if (connect == nullptr) {
        control = cephalopod_pipe::PortState::CLOSE;
        return;
    }
    try {
        connect->recv(data);
    } catch (const std::string& e) {
        control = cephalopod_pipe::PortState::CLOSE;
    }
}

void NetworkConnection::doSend(const std::string& data, cephalopod_pipe::PortState& control) {
    if (connect == nullptr) {
        control = cephalopod_pipe::PortState::CLOSE;
        return;
    }
    try {
        connect->send(data);
    } catch (const std::string& e) {
        control = cephalopod_pipe::PortState::CLOSE;
    }
}

void NetworkConnection::close() {
}

std::string NetworkConnection::getFileName(unsigned long id) {
    return std::string("cache/") + uuid + "-" + std::to_string(id);
}

void TransmissionNetworkConnection::send(const std::string& data, cephalopod_pipe::PortState& control) {
    doSend(data, control);
}

void ProxyLogicNetworkConnection::recv(std::string& data, cephalopod_pipe::PortState& control) {
    std::unique_lock<std::mutex> lock(mt);
    cv.wait(lock, [this]() {
        if (errorMessageSendback != "") {
            return true;
        }
        if (connectSuccessMessage != "") {
            return true;
        }
        if (connect->isOpen()) {
            return true;
        }
        return false;
    });
    if (errorMessageSendback != "") {
        data = "HTTP/1.1 503 Service Unavailable\r\n";
        data += "Content-Type: text/plain; charset=UTF-8\r\n";
        data += "Content-Length: " + std::to_string(errorMessageSendback.length()) + "\r\n";
        data += "\r\n";
        data += errorMessageSendback;
        errorMessageSendback.clear();
        return;
    }
    if (connectSuccessMessage != "") {
        data = connectSuccessMessage;
        connectSuccessMessage.clear();
        return;
    }
    if (connect->isOpen()) {
        lock.unlock();
        doRecv(data, control);
        return;
    }
    control = cephalopod_pipe::PortState::CLOSE;
}

void ProxyLogicNetworkConnection::send(const std::string& data, cephalopod_pipe::PortState& control) {
    std::unique_lock<std::mutex> lock(mt);
    if (connect->isOpen() && httpRequest.isConnect()) {
        cv.notify_one();
        lock.unlock();
        doSend(data, control);
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
            auto host = httpRequest.getConnectHost();
            auto port = httpRequest.getConnectPort();

            if (httpRequest.isConnect()) {
                try {
                    openWithRetry(host, port, 2);
                } catch (const std::string& errorMessage) {
                    errorMessageSendback = errorMessage;
                    control = cephalopod_pipe::PortState::CLOSE;
                    cv.notify_one();
                    return;
                }
                connectSuccessMessage = "HTTP/1.1 200 Connection Established\r\n\r\n";
                if (offset >= dataLength - 1) {
                    cv.notify_one();
                    return;
                }
                auto dataRealSend = data.substr(offset);
                doSend(dataRealSend, control);
                cv.notify_one();
                return;
            }

            httpRequest.removeProxyInfo();
            auto dataRealSend = httpRequest.getRequestContent();
            if (connect->isOpen()) {
                if (host != firstRequestHost || port != firstRequestPort) {
                    control = cephalopod_pipe::PortState::CLOSE;
                    cv.notify_one();
                    return;
                }
                doSend(dataRealSend, control);
                if (control == cephalopod_pipe::PortState::CLOSE) {
                    cv.notify_one();
                    return;
                }
                continue;
            }
            try {
                openWithRetry(host, port, 2);
            } catch (const std::string& errorMessage) {
                errorMessageSendback = errorMessage;
                control = cephalopod_pipe::PortState::CLOSE;
                cv.notify_one();
                return;
            }
            doSend(dataRealSend, control);
            if (control == cephalopod_pipe::PortState::CLOSE) {
                cv.notify_one();
                return;
            }
            httpResolved = false;
            firstRequestHost = host;
            firstRequestPort = port;
        }
    }
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
