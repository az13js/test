#ifndef _cephalopod_http_h_
#define _cephalopod_http_h_

#include "cephalopod_http_tools.h"
#include <string>
#include <functional>
#include <vector>

namespace cephalopod_http {

    class HTTP {
    public:
        cephalopod_http_tools::FirstData hf = cephalopod_http_tools::FirstData();
        std::vector<cephalopod_http_tools::HeadData> hv = std::vector<cephalopod_http_tools::HeadData>();
        std::string body = "";
        unsigned long getContentLength() const;
        bool isConnect() const;
        std::string getConnectHost() const;
        int getConnectPort() const;
        bool isUrlFull() const;
        bool haveProxyConnection() const;
        void removeProxyInfo();
        std::string getRequestContent() const;
    };

    class HTTPPaser {
    public:
        void apply(const std::string& data, const std::function<void(const HTTP&)>& callback);
    private:
        bool processingHeader = true;
        cephalopod_http_tools::State s = cephalopod_http_tools::State::S_DEFAULT;
        std::string raw = "";
        bool isFirst = true;
        HTTP http;
    };

} // namespace cephalopod_http

#endif // _cephalopod_http_h_

/* EXAMPLE

#include "cephalopod_http.h"
#include <iostream>
#include <string>
#include <functional>

int main() {
    using namespace std;
    using namespace cephalopod_http;

    string data;
    data += "GET / HTTP/1.1\r\n";
    data += "Host: test1.com\r\n";
    data += "Content-Length: 5\r\n";
    data += "\r\n";
    data += "Hello";

    data += "GET /Hello HTTP/1.1\r\n";
    data += "Host: test2.com\r\n";
    data += "\r\n";

    auto callback = function<void(const HTTP&)>([](const HTTP& http) {
        cout << "---------- BEGIN ----------" << endl;
        cout << "HTTP in, url=" << http.hf.url << ", ";
        cout << "content length: " << http.getContentLength() << ", ";
        cout << "content: #" << http.body << "#" << endl;
        cout << "First Line:" << endl;
        cout << http.hf.method << " " << http.hf.url << " " << http.hf.version << endl;
        cout << "Headers:" << endl;
        for (const auto& head : http.hv) {
            cout << "(" << endl;
            cout << "  Field #" << head.field << "#" << endl;
            cout << "  Value #" << head.value << "#" << endl;
            cout << ")" << endl;
        }
        cout << "---------- END ----------" << endl;
    });
    HTTPPaser p;
    p.apply(data, callback);
    return 0;
}

*/
