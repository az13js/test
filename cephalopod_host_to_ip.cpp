#include "cephalopod_host_to_ip.h"

// getaddrinfo, gai_strerror, freeaddrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// inet_ntop
#include <arpa/inet.h>

// strerror
#include <cstring>

// for cache
#include <mutex>
#include <map>

namespace cephalopod_host_to_ip {

std::vector<std::string> hostToIPs(const std::string& host) {
    std::vector<std::string> addressList;

    addrinfo hints;
#ifdef CEPHALOPOD_NO_IPV6_SUPPORT
    hints.ai_family = AF_INET;
#else
    hints.ai_family = AF_UNSPEC;
#endif
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;

    addrinfo* result = nullptr;

    auto res = getaddrinfo(host.c_str(), nullptr, &hints, &result);
    if (0 != res) {
        throw std::string(gai_strerror(res));
    }

    if (nullptr != result) {
        int bufferSize = 64;
        char buffer[64];
        void* pointer;

        for (auto rp = result; rp != nullptr; rp = rp->ai_next) {
            switch (rp->ai_family) {
            case AF_INET:
                pointer = (void*)&(((sockaddr_in*)(rp->ai_addr))->sin_addr);
                break;
#ifndef CEPHALOPOD_NO_IPV6_SUPPORT
            case AF_INET6:
                pointer = (void*)&(((sockaddr_in6*)(rp->ai_addr))->sin6_addr);
                break;
#endif
            default:
                continue;
            }
            if (nullptr == inet_ntop(rp->ai_family, pointer, buffer, bufferSize)) {
                freeaddrinfo(result);
                throw std::string("Run inet_ntop fail, errno is ") + std::to_string(errno)
                    + ", message: " + std::string(strerror(errno));
            }
            addressList.emplace_back(std::string(buffer));
        }
        freeaddrinfo(result);
    }

    if (addressList.size() <= 0) {
        throw std::string("address list is empty");
    }
    return addressList;
}

std::vector<std::string> hostToIPsCache(const std::string& host) {
    static std::mutex mt;
    static std::map<std::string, std::vector<std::string>> domainIpsMap;
    std::unique_lock<std::mutex> lock(mt);
    if (domainIpsMap.end() == domainIpsMap.find(host)) {
        lock.unlock();
        auto ips = hostToIPs(host);
        lock.lock();
        domainIpsMap[host] = ips;
        lock.unlock();
        return ips;
    }
    return domainIpsMap[host];
}

} // namespace cephalopod_host_to_ip

