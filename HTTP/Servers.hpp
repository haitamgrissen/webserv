#ifndef Servers_hpp
#define Servers_hpp

#include <stdio.h>
#include <sys/socket.h>
# include <netinet/in.h>
#include <map>
#include <string>
#include <iterator>
#include <vector>
#include "Location.hpp"
#include "Cgi.hpp"
#include "Conf.hpp"

class Servers
{
    private:
        std::vector<Conf>               server;
        int                             server_block;

    public:
        Servers(void);
        //Servers(std::string File_name);
        std::vector<Conf>               get_server();
        void                            parse_server(std::string name);
        ~Servers(void);
};

#endif 