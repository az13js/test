#include "cephalopod_http_tools.h"

namespace cephalopod_http_tools {

    void applyChar(const char& ch, State& s, std::string& header, const std::function<void(void)>& callback) {
        switch (s) {
        case State::S_DEFAULT:
            switch (ch) {
            case '\r':
                s = State::S_RETURN;
                break;
            default:
                header += ch;
            }
            break;
        case State::S_RETURN:
            s = State::S_DEFAULT;
            switch (ch) {
            case '\n':
                callback();
                header = "";
                break;
            default:
                header += '\r';
                header += ch;
            }
            break;
        }
    }

    void parserFirst(const std::string& raw, FirstData& firstData) {
        int s = 1;
        for (const auto& ch : raw) {
            switch (s) {
            case 1:
                if (' ' != ch) {
                    firstData.method += ch;
                    s = 2;
                }
                break;
            case 2:
                if (' ' == ch) {
                    s = 3;
                }
                else {
                    firstData.method += ch;
                }
                break;
            case 3:
                if (' ' != ch) {
                    firstData.url += ch;
                    s = 4;
                }
                break;
            case 4:
                if (' ' == ch) {
                    s = 5;
                }
                else {
                    firstData.url += ch;
                }
                break;
            case 5:
                if (' ' != ch) {
                    firstData.version += ch;
                    s = 6;
                }
                break;
            case 6:
                firstData.version += ch;
                break;
            }
        }
    }

    void parserHead(const std::string& raw, HeadData& headData) {
        int s = 1;
        for (const auto& ch : raw) {
            switch (s) {
            case 1:
                switch (ch) {
                case ' ':
                    break;
                case ':':
                    s = 3;
                    break;
                default:
                    headData.field += ch;
                    s = 2;
                }
                break;
            case 2:
                if (':' == ch) {
                    s = 3;
                }
                else {
                    headData.field += ch;
                }
                break;
            case 3:
                if (' ' != ch) {
                    s = 4;
                    headData.value += ch;
                }
                break;
            case 4:
                headData.value += ch;
                break;
            }
        }
    }

} // namespace cephalopod_http_tools
