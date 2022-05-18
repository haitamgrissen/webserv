#pragma once

#include "utilities_.hpp"
#include "Server.hpp"
#include "ServerGroup.hpp"


class DebugWS
{
    public :

    std::vector<int> getServerPorts()
    {
        std::vector<int> tmp;
        tmp.push_back(7333);
        tmp.push_back(8084);
        // tmp.push_back(4430);
        return tmp;
    }

    std::vector<std::string> getServerHosts()
    {
        std::vector<std::string> tmp;
        tmp.push_back("127.0.0.1");
        tmp.push_back("10.11.1.3");
        // tmp.push_back("127.0.0.50");
        return tmp;
    }
};