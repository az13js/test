#ifndef _cephalopod_thread_safe_queue_h_
#define _cephalopod_thread_safe_queue_h_

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace cephalopod_thread_safe_queue {

    class StringQueue {
    public:
        void push(const std::string& str);
        std::string frontAndPop();

    private:
        std::queue<std::string> stringQueue;
        std::mutex mt;
        std::condition_variable cv;
    };

    class PointerQueue {
    public:
        void push(void* pointer);
        void* frontAndPop();

    private:
        std::queue<void*> pointerQueue;
        std::mutex mt;
        std::condition_variable cv;
    };

} // namespace cephalopod_thread_safe_queue

#endif // _cephalopod_thread_safe_queue_h_
