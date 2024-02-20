#include "cephalopod_thread_safe_queue.h"

namespace cephalopod_thread_safe_queue {

    void StringQueue::push(const std::string& str) {
        std::unique_lock<std::mutex> lock(mt);
        stringQueue.push(str);
        cv.notify_one();
    }

    std::string StringQueue::frontAndPop() {
        std::unique_lock<std::mutex> lock(mt);
        cv.wait(lock, [this]() { return !stringQueue.empty(); });
        std::string result = stringQueue.front();
        stringQueue.pop();
        return result;
    }

    void LongQueue::push(long s) {
        std::unique_lock<std::mutex> lock(mt);
        longQueue.push(s);
        cv.notify_one();
    }

    long LongQueue::frontAndPop() {
        std::unique_lock<std::mutex> lock(mt);
        cv.wait(lock, [this]() { return !longQueue.empty(); });
        long result = longQueue.front();
        longQueue.pop();
        return result;
    }

} // namespace cephalopod_thread_safe_queue
