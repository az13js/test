#include "cephalopod_http.h"
#include <stdexcept>

namespace cephalopod_http {

    unsigned long HTTP::getContentLength() const {
        using namespace std;
        unsigned long contentLength = 0;
        for (const auto& h : hv) {
            std::string f;
            for (const auto& ch : h.field) {
                f += tolower(ch);
            }
            if (f == "content-length") {
                contentLength = stol(h.value);
            }
        }
        return contentLength;
    }

    bool HTTP::isConnect() const {
        using namespace std;
        string method;
        for (const auto& ch : hf.method) {
            method += tolower(ch);
        }
        return method == "connect";
    }

    std::string HTTP::getConnectHost() const {
        using namespace std;
        if (isConnect()) {
            // for IPv6
            auto ipv6Begin = hf.url.find('[');
            if (0 == ipv6Begin) {
                auto end = hf.url.find(']');
                if (string::npos == end) {
                    return "";
                }
                return hf.url.substr(1, end - ipv6Begin - 1);
            }
            auto mao = hf.url.find(':');
            if (mao == string::npos) {
                return hf.url;
            }
            return hf.url.substr(0, mao);
        }
        if (isUrlFull()) {
            // for IPv6
            auto ipv6Begin = hf.url.find('[');
            if (string::npos != ipv6Begin) {
                auto end = hf.url.find(']');
                if (string::npos == end) {
                    return "";
                }
                return hf.url.substr(ipv6Begin + 1, end - ipv6Begin - 1);
            }

            auto pm = hf.url.find("://");
            auto end = hf.url.find('/', pm + 3);
            if (end == string::npos) {
                return hf.url.substr(pm + 3);
            }
            return hf.url.substr(pm + 3, end - pm - 3);
        }
        string host;
        for (const auto& h : hv) {
            std::string f;
            for (const auto& ch : h.field) {
                f += tolower(ch);
            }
            if (f == "host") {
                host = h.value;
            }
        }

        // for IPv6
        auto ipv6Begin = host.find('[');
        if (string::npos != ipv6Begin) {
            auto end = host.find(']');
            if (string::npos == end) {
                return "";
            }
            return host.substr(ipv6Begin + 1, end - ipv6Begin - 1);
        }

        auto n = host.find(':');
        if (n == string::npos) {
            return host;
        }
        return host.substr(0, n);
    }

    int HTTP::getConnectPort() const {
        using namespace std;
        try {
            if (isConnect()) {
                auto ipv6End = hf.url.find("]:");
                if (string::npos != ipv6End) {
                    return stoi(hf.url.substr(ipv6End + 2));
                }
                ipv6End = hf.url.find("]");
                if (string::npos != ipv6End) {
                    return 80;
                }

                auto mao = hf.url.find(':');
                if (mao == string::npos) {
                    return 80;
                }
                return stoi(hf.url.substr(mao + 1));
            }
            if (isUrlFull()) {
                auto ipv6End = hf.url.find("]:");
                if (string::npos != ipv6End) {
                    auto end = hf.url.find('/', ipv6End + 2);
                    if (end == string::npos) {
                        return stoi(hf.url.substr(ipv6End + 2));
                    }
                    return stoi(hf.url.substr(ipv6End + 2, end - ipv6End - 1));
                }
                ipv6End = hf.url.find("]");
                if (string::npos != ipv6End) {
                    return 80;
                }

                auto pm = hf.url.find("://");
                auto mao = hf.url.find(':', pm + 3);
                if (mao == string::npos) {
                    return 80;
                }
                auto end = hf.url.find('/', pm + 3);
                if (end == string::npos) {
                    return stoi(hf.url.substr(mao + 1));
                }
                if (end < mao) {
                    return 80;
                }
                return stoi(hf.url.substr(mao + 1, end - mao));
            }
            string host;
            for (const auto& h : hv) {
                std::string f;
                for (const auto& ch : h.field) {
                    f += tolower(ch);
                }
                if (f == "host") {
                    host = h.value;
                }
            }
            auto ipv6End = host.find("]:");
            if (string::npos != ipv6End) {
                return stoi(host.substr(ipv6End + 2));
            }
            ipv6End = host.find("]");
            if (string::npos != ipv6End) {
                return 80;
            }

            auto n = host.find(':');
            if (n == string::npos) {
                return 80;
            }
            return stoi(host.substr(n + 1));
        } catch (const invalid_argument &e) {
        }
        return 80;
    }

    bool HTTP::isUrlFull() const {
        using namespace std;
        auto pm = hf.url.find("://");
        if (pm == string::npos) {
            return false;
        }
        return true;
    }

    bool HTTP::haveProxyConnection() const {
        using namespace std;
        for (const auto& h : hv) {
            std::string f;
            for (const auto& ch : h.field) {
                f += tolower(ch);
            }
            if (f == "proxy-connection") {
                return true;
            }
        }
        return false;
    }

    void HTTP::removeProxyInfo() {
        std::string newUrl;
        auto pm = hf.url.find("://");
        if (pm != std::string::npos) {
            auto end = hf.url.find('/', pm + 3);
            if (end != std::string::npos) {
                hf.url = hf.url.substr(end);
            }
        }
        std::vector<cephalopod_http_tools::HeadData> newHv;
        for (const auto& h : hv) {
            std::string f;
            for (const auto& ch : h.field) {
                f += tolower(ch);
            }
            if (f == "proxy-connection") {
                auto newHd = h;
                newHd.field = "Connection";
                newHv.emplace_back(newHd);
                continue;
            }
            newHv.emplace_back(h);
        }
        hv = newHv;
    }

    std::string HTTP::getRequestContent() const {
        std::string result;
        result += hf.method + " " + hf.url + " " + hf.version + "\r\n";
        for (const auto& v : hv) {
            result += v.field + ": " + v.value + "\r\n";
        }
        result += "\r\n";
        return result + body;
    }

    void HTTPPaser::apply(const std::string& data, const std::function<void(const HTTP&)>& callback) {
        using namespace cephalopod_http_tools;
        auto doCallback = [this, &callback]() {
            callback(http);
            processingHeader = true;
            s = State::S_DEFAULT;
            isFirst = true;
            http = HTTP();
        };
        std::function<void(void)> applyCharCallback = [this, &doCallback]() {
            if (raw == "") {
                processingHeader = false;
                if (0 == http.getContentLength()) {
                    doCallback();
                }
                return;
            }
            if (isFirst) {
                isFirst = false;
                parserFirst(raw, http.hf);
                return;
            }
            HeadData hd;
            parserHead(raw, hd);
            http.hv.emplace_back(hd);
        };
        for (const auto& ch : data) {
            if (processingHeader) {
                applyChar(ch, s, raw, applyCharCallback);
                continue;
            }
            http.body += ch;
            if (http.body.length() >= http.getContentLength()) {
                doCallback();
            }
        }
    }
}

