#ifndef _cephalopod_pipe_h_
#define _cephalopod_pipe_h_

#include <string>

namespace cephalopod_pipe {

    enum PortState { NORMAL, CLOSE };

    class Port {
    public:
        virtual void recv(std::string& data, PortState& control) = 0;
        virtual void send(const std::string& data, PortState& control) = 0;
        virtual void close() = 0;
        virtual std::string getFileName(unsigned long id) = 0;
        virtual ~Port() {}
    };

    void makePipe(Port* p1, Port* p2);
    void makePipe(Port* p1, Port* p2, bool isJoin);

} // namespace cephalopod_pipe

#endif // _cephalopod_pipe_h_

/* EXAMPLE

#include "cephalopod_pipe.h"
#include <iostream>
#include <string>
#include <mutex>

using namespace cephalopod_pipe;

std::mutex mt;

class My :public Port {
public:
    int p;
    int n = 0;
    My(int p) {
        this->p = p;
    }

    void recv(std::string& data, PortState& control) {
        if (n < 10) {
            data = std::string("This is data from ") + std::to_string(p) + " and data n = " + std::to_string(n);
            n++;
        }
        else {
            control = PortState::CLOSE;
        }
    }
    void send(std::string& data, PortState& control) {
        std::unique_lock<std::mutex> lock(mt);
        std::cout << "The data send to p" << p << ":" << data << std::endl;
    }

    void close() {
        std::unique_lock<std::mutex> lock(mt);
        std::cout << "p" << p << " close" << std::endl;
    }

    std::string getFileName(unsigned long id) {
        return std::string("ft") + std::to_string(p) + std::string("f") + std::to_string(id) + ".txt";
    }
};

int main() {
    int x;
    for (x = 0; x < 1; x++) {
        makePipe(new My(2 * x + 1), new My(2 * x + 2));
    }

    std::string i;
    std::cin >> i;

    return 0;
}

*/
