#include "cephalopod_argument.h"

namespace cephalopod_argument {

    Parameters::Parameters() {
    }

    Parameters::Parameters(int argc, const char *argv[]) {
        parse(argc, argv);
    }

    void Parameters::parse(int argc, const char *argv[]) {
        using namespace std;
        params.clear();
        string tmp;
        string key;
        string value;
        for (int idx = 0; idx < argc; idx++) {
            tmp.assign(argv[idx]);
            if (0 == tmp.find("--")) {
                auto eqOffset = tmp.find("=");
                if (string::npos != eqOffset) {
                    key.assign(argv[idx], 2, eqOffset - 2);
                    value.assign(tmp.substr(eqOffset + 1));
                }
                else {
                    key.assign(tmp.substr(2));
                    value.clear();
                }
                params[key] = value;
            }
        }
    }

    bool Parameters::paramExists(const std::string& key) const {
        return params.end() != params.find(key);
    }

    std::string Parameters::getParamValue(const std::string& key) const {
        if (!paramExists(key)) {
            return "";
        }
        return params.at(key);
    }

} // namespace cephalopod_argument
