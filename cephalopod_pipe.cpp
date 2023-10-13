#include "cephalopod_pipe.h"
#include "cephalopod_thread_safe_queue.h"
#include <thread>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace cephalopod_pipe {

    void makePipe(Port* p1, Port* p2) {
        makePipe(p1, p2, false);
    }

    void makePipe(Port* p1, Port* p2, bool isJoin) {
        using namespace std;
        using cephalopod_thread_safe_queue::StringQueue;
        auto mainFunction = [](Port* p1, Port* p2) {
            StringQueue p1Recv, p1File, p2Recv, p2File;
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
                    StringQueue* queueFile
                ) {
                    string recvData;
                    unsigned long id = 0;
                    do {
                        recvData = queueRecv->frontAndPop();
                        auto fileName = p->getFileName(id++);
                        ofstream file(fileName, ios::out | ios::binary);
                        file << recvData;
                        file.close();
                        queueFile->push(fileName);
                    } while (recvData != "");
                };
            auto threadSendLogic = [](
                    Port* pSend,
                    StringQueue* queueFile
                ) {
                    string fileData;
                    auto s = PortState::NORMAL;
                    do {
                        auto fileName = queueFile->frontAndPop();
                        ifstream file(fileName, ios::in | ios::binary);
                        stringstream buffer;
                        buffer << file.rdbuf();
                        fileData = buffer.str();
                        file.close();
                        filesystem::remove(filesystem::path(fileName));
                        if (fileData != "" && s != PortState::CLOSE) {
                            pSend->send(fileData, s);
                        }
                    } while (fileData != "");
                    pSend->close();
                };
            try {
                thread p1ThreadRecv(threadRecvLogic, p1, &p1Recv);
                thread p2ThreadRecv(threadRecvLogic, p2, &p2Recv);
                thread p1ThreadFile(threadFileLogic, p1, &p1Recv, &p1File);
                thread p2ThreadFile(threadFileLogic, p2, &p2Recv, &p2File);
                thread p1ThreadSend(threadSendLogic, p2, &p1File);
                thread p2ThreadSend(threadSendLogic, p1, &p2File);
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
