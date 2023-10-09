#ifndef _cephalopod_host_to_ip_h_
#define _cephalopod_host_to_ip_h_

#include <string>
#include <vector>

namespace cephalopod_host_to_ip {

std::vector<std::string> hostToIPs(const std::string& host);

std::vector<std::string> hostToIPsCache(const std::string& host);

}

#endif // _cephalopod_host_to_ip_h_

