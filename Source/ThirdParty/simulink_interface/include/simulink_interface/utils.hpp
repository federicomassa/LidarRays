#pragma once

#include <string>
#include <regex>

namespace simulink {

    bool is_ip_address(const std::string& endpoint) {
        const std::regex ip_re("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?).){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
        return std::regex_match(endpoint, ip_re);
    }

    bool is_broadcast_ip_address(const std::string& endpoint){
        const std::regex brd_ip_re("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?).){3}(255)$");
        return std::regex_match(endpoint, brd_ip_re);
    }
    
} // simulink
