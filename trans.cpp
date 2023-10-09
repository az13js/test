#include "logic.h"
#include "cephalopod_argument.h"
#include <string>
#include <iostream>

void printHelpMessage() {
    using namespace std;
    cout << "Forwarding is achieved by specifying the listening port, target IP address, and port." << endl;
    cout << "For example:" << endl;
    cout << "./trans --listen-ip=0.0.0.0 --listen-port=80 --target-ip=192.168.0.3 --target-port=80" << endl;
    cout << "IPv6 compatible." << endl;
}

int main(int argc, const char *argv[]) {
    cephalopod_argument::Parameters parameters(argc, argv);
    auto listenIp = parameters.getParamValue("listen-ip");
    auto listenPort = parameters.getParamValue("listen-port");
    auto targetIp = parameters.getParamValue("target-ip");
    auto targetPort = parameters.getParamValue("target-port");

    if (listenIp == "" || listenPort == "" || targetIp == "" || targetPort == "") {
        printHelpMessage();
        return 1;
    }

    try {
        std::function<void(cephalopod_black_hole::Connect&)> callback = [&targetIp, &targetPort](cephalopod_black_hole::Connect& connect) {
            cephalopod_black_hole::Connect dstConnect;
            try {
                dstConnect.open(targetIp, std::stoi(targetPort), true);
            } catch (const std::string& e) {
            }
            if (!dstConnect.isOpen()) {
                return;
            }
            auto src = new TransmissionNetworkConnection();
            src->bind(&connect);
            auto dst = new TransmissionNetworkConnection();
            dst->bind(&dstConnect);
            makePipe(src, dst, true);
        };
        cephalopod_black_hole::BlackHole(listenIp, std::stoi(listenPort), callback);
    } catch (const std::string& e) {
        std::cout << "ERROR: " << e << std::endl;
    }
    return 0;
}
