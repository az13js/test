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

    void PointerQueue::push(void* pointer) {
        std::unique_lock<std::mutex> lock(mt);
        pointerQueue.push(pointer);
        cv.notify_one();
    }

    void* PointerQueue::frontAndPop() {
        std::unique_lock<std::mutex> lock(mt);
        cv.wait(lock, [this]() { return !pointerQueue.empty(); });
        void* result = pointerQueue.front();
        pointerQueue.pop();
        return result;
    }

} // namespace cephalopod_thread_safe_queue
