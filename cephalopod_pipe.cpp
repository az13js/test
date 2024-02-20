#include "cephalopod_pipe.h"
#include "cephalopod_thread_safe_queue.h"
#include <thread>
#include <fstream>
#include <filesystem>

namespace cephalopod_pipe {

    void makePipe(Port* p1, Port* p2) {
        makePipe(p1, p2, false);
    }

    void makePipe(Port* p1, Port* p2, bool isJoin) {
        using namespace std;
        using cephalopod_thread_safe_queue::StringQueue;
        using cephalopod_thread_safe_queue::LongQueue;
        auto mainFunction = [](Port* p1, Port* p2) {
            StringQueue p1Recv, p2Recv;
            LongQueue p1Size, p2Size;
            auto threadRecvLogic = [](
                    Port* p,
                    StringQueue* queueRecv
                ) {
                    string data;
                    auto s = PortState::NORMAL;
                    do {
                        p->recv(data, s);
                        if (data != "") {
                            queueRecv->push(data);
                            data.clear();
                        }
                        if (s == PortState::CLOSE) {
                            queueRecv->push(string(""));
                        }
                    } while (s != PortState::CLOSE);
                };
            auto threadFileLogic = [](
                    Port* p,
                    StringQueue* queueRecv,
                    LongQueue* queueSize
                ) {
                    string recvData;
                    ofstream file(p->getFileName(), ios::out | ios::binary);
                    do {
                        recvData = queueRecv->frontAndPop();
                        file << recvData;
                        file.flush();
                        queueSize->push(recvData.length());
                    } while (recvData != "");
                };
            auto threadSendLogic = [](
                    Port* pRecv,
                    Port* pSend,
                    LongQueue* queueSize
                ) {
                    string fileData;
                    auto s = PortState::NORMAL;
                    auto fileName = pRecv->getFileName();
                    ifstream file(fileName, ios::in | ios::binary);
                    while (!file.is_open()) {
                        file = ifstream(fileName, ios::in | ios::binary);
                    }
                    char *buf = nullptr;
                    do {
                        auto size = queueSize->frontAndPop();
                        buf = new char[size];
                        file.read(buf, size);
                        fileData.assign(buf, size);
                        delete[] buf;
                        if (fileData != "" && s != PortState::CLOSE) {
                            pSend->send(fileData, s);
                        }
                    } while (fileData != "");
                    pSend->close();
                    filesystem::remove(filesystem::path(fileName));
                };
            try {
                thread p1ThreadRecv(threadRecvLogic, p1, &p1Recv);
                thread p2ThreadRecv(threadRecvLogic, p2, &p2Recv);
                thread p1ThreadFile(threadFileLogic, p1, &p1Recv, &p1Size);
                thread p2ThreadFile(threadFileLogic, p2, &p2Recv, &p2Size);
                thread p1ThreadSend(threadSendLogic, p1, p2, &p1Size);
                thread p2ThreadSend(threadSendLogic, p2, p1, &p2Size);
                p1ThreadRecv.join();
                p2ThreadRecv.join();
                p1ThreadFile.join();
                p2ThreadFile.join();
                p1ThreadSend.join();
                p2ThreadSend.join();
            } catch (const system_error& e) {
            }
            delete p1;
            delete p2;
        };
        if (isJoin) {
            mainFunction(p1, p2);
        } else {
            try {
                thread worker(mainFunction, p1, p2);
                worker.detach();
            } catch (const system_error& e) {
            }
        }
    }

} // namespace cephalopod_pipe
