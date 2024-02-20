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
        virtual std::string getFileName() = 0;
        virtual ~Port() {}
    };

    void makePipe(Port* p1, Port* p2);
    void makePipe(Port* p1, Port* p2, bool isJoin);

} // namespace cephalopod_pipe

#endif // _cephalopod_pipe_h_
