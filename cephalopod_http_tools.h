#ifndef _cephalopod_http_tools_h_
#define _cephalopod_http_tools_h_

#include <string>
#include <functional>

namespace cephalopod_http_tools {

    /********************* tool 1 *********************/

    enum State { S_DEFAULT, S_RETURN };

    void applyChar(const char& ch, State& s, std::string& header, const std::function<void(void)>& callback);

    /********************* tool 2 *********************/

    struct FirstData {
        std::string method  = "";
        std::string url     = "";
        std::string version = "";
    };

    void parserFirst(const std::string& raw, FirstData& firstData);

    /********************* tool 3 *********************/

    struct HeadData {
        std::string field = "";
        std::string value = "";
    };

    void parserHead(const std::string& raw, HeadData& headData);

} // namespace cephalopod_http_tools

#endif // _cephalopod_http_tools_h_
