#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "cephalopod_log.h"
#include "cephalopod_thread_safe_queue.h"
#include <thread>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace cephalopod_log {

    LogUtil& LogUtil::getLogUtil() {
        return logUtil;
    }

    void LogUtil::info(const std::string& message) {
        auto q = (cephalopod_thread_safe_queue::StringQueue*)queue;
        q->push(getTimeString() + " " + message);
    }

    void LogUtil::info(const std::string&& message) {
        auto q = (cephalopod_thread_safe_queue::StringQueue*)queue;
        q->push(getTimeString() + " " + message);
    }

    void LogUtil::info(const char* message) {
        auto q = (cephalopod_thread_safe_queue::StringQueue*)queue;
        q->push(getTimeString() + " " + message);
    }

    LogUtil LogUtil::logUtil;

    LogUtil::LogUtil() {
        queue = (void*)(new cephalopod_thread_safe_queue::StringQueue());
        thread = (void*)(new std::thread([this]() {
            auto q = (cephalopod_thread_safe_queue::StringQueue*)queue;
            std::string message;
            std::ofstream outputFile;
            do {
                message = q->frontAndPop();
                if (message != "") {
                    outputFile.open("cache/debug.log", std::ofstream::out | std::ofstream::app);
                    if (!outputFile.is_open()) {
                        continue;
                    }
                    outputFile << message << std::endl;
                    outputFile.close();
                }
            } while (message != "");
        }));
    }

    LogUtil::~LogUtil() {
        auto q = (cephalopod_thread_safe_queue::StringQueue*)queue;
        q->push(std::string());
        auto t = (std::thread*)thread;
        t->join();
        delete t;
        delete q;
    }

    std::string LogUtil::getTimeString() const {
        using namespace std;
        stringstream ss;
        auto nowTime = time(nullptr);
        ss << put_time(localtime(&nowTime), "%F %X %z");
        return ss.str();
    }

} // namespace cephalopod_log
