#include "logic.h"
#include "cephalopod_argument.h"
#include <string>
#include <iostream>
#include <filesystem>

void printHelpMessage() {
    using namespace std;
    cout << "To use this command, you need to specify the IP address and port to listen on." << endl;
    cout << "For example:" << endl;
    cout << "./proxy --listen-ip=0.0.0.0 --listen-port=80" << endl;
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

    try {
        std::function<void(cephalopod_black_hole::Connect&)> callback = [](cephalopod_black_hole::Connect& connect) {
            auto src = new TransmissionNetworkConnection();
            src->bind(&connect);
            auto dst = new ProxyLogicNetworkConnection();
            cephalopod_black_hole::Connect dstConnect;
            dst->bind(&dstConnect);
            makePipe(src, dst, true);
        };
        cephalopod_black_hole::BlackHole(listenIp, std::stoi(listenPort), callback);
    } catch (const std::string& e) {
        std::cout << "ERROR: " << e << std::endl;
        return 1;
    }
    return 0;
}
