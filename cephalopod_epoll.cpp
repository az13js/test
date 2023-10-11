#include "cephalopod_epoll.h"
#include "cephalopod_black_hole.h"

namespace cephalopod_epoll {

void EpollServer::start(const std::string& address, int port) {
    auto getUnblockFileDescriptor = [](const std::string& address, int port) {
        cephalopod_black_hole::BlackHole fileDescriptorCreater(address, port, true);
        fileDescriptorCreater.setUnBlock();
        return fileDescriptorCreater.getFileDescriptor();
    };

    auto fileDescriptor = getUnblockFileDescriptor(address, port);
}

} // namespace cephalopod_epoll
