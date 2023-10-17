#ifndef _cephalopod_log_h_
#define _cephalopod_log_h_

#include <string>

namespace cephalopod_log {

    class LogUtil final {
    public:
        static LogUtil& getLogUtil();
        void info(const std::string& message);
        void info(const std::string&& message);
        void info(const char* message);
    private:
        void* queue;
        void* thread;
        static LogUtil logUtil;
        LogUtil();
        ~LogUtil();
        std::string getTimeString() const;
    };

} // namespace cephalopod_log

#endif // _cephalopod_log_h_
