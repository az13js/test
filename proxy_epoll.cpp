#include "cephalopod_argument.h"
#include "cephalopod_epoll.h"
#include <string>
#include <iostream>
#include <filesystem>

void printHelpMessage() {
    using namespace std;
    cout << "To use this command, you need to specify the IP address and port to listen on." << endl;
    cout << "For example:" << endl;
    cout << "./proxy_epoll --listen-ip=0.0.0.0 --listen-port=80" << endl;
    cout << "IPv6 compatible." << endl;
}

int main(int argc, const char *argv[]) {
    cephalopod_argument::Parameters parameters(argc, argv);
    auto listenIp = parameters.getParamValue("listen-ip");
    auto listenPort = parameters.getParamValue("listen-port");

    if (listenIp == "" || listenPort == "") {
        printHelpMessage();
        return 1;
    }

    if (!std::filesystem::is_directory(std::filesystem::path("cache"))) {
        std::cerr << "No directory: cache" << std::endl;
        return 1;
    }

    std::cout << "Ok, --listen-ip=\"" << listenIp << "\" --listen-port=\"" << listenPort << "\"" << std::endl;

    cephalopod_epoll::EpollServer server;
    try {
        server.start(listenIp, std::stoi(listenPort));
    } catch (const std::string& e) {
        std::cout << e << std::endl;
    }

    std::string tmp;
    std::cin >> tmp;
    std::cout << "Finish, your input is " << tmp << std::endl;
    return 0;
}
