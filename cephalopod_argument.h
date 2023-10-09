#ifndef _cephalopod_argument_h_
#define _cephalopod_argument_h_

#include <map>
#include <string>

namespace cephalopod_argument {

    class Parameters {
    public:
        Parameters();
        Parameters(int argc, const char *argv[]);
        void parse(int argc, const char *argv[]);
        bool paramExists(const std::string& key) const;
        std::string getParamValue(const std::string& key) const;
    private:
        std::map<std::string, std::string> params;
    };

} // namespace cephalopod_argument

#endif // _cephalopod_argument_h_
