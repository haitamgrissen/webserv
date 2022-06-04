#include "Conf.hpp"
#include "Location.hpp"
#include "Cgi.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include "Servers.hpp"


#include <iostream>
#include <unistd.h>
#include <cstring>
#include <map>
#include <cstdlib>
#include <vector>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <istream>
#include <sys/types.h>
#include <signal.h>
#include <string>
#include <algorithm>

Servers::Servers(void)
{
}

void               Servers::parse_server(std::string name)
{
    std::string     buff;
    std::ifstream   file(name);
    Cgi             *ok_cgi = new Cgi;
    Location        *ok = new Location;
    Conf            *my_conf = new Conf();

    my_conf->set_location_block(0);
    my_conf->set_cgi_block(0);
    this->server_block = 0;
    while(getline(file, buff))
    {
        std::string     str;
        std::vector <std::string> tokens;
        std::stringstream check(buff);
        size_t i = 0;
        std::string       t = "/";
       
        int             empty_line;
        while(getline(check, str, ' '))
        {
            if (str.size() != 0)
            {
                empty_line = 1;
                if (str == "location")
                {
                    my_conf->set_location_block(1);
                }   
                else if (str == "cgi")
                    my_conf->set_cgi_block(1);
                else if (str == "Server")
                {
                    this->server_block = 1;
                }                    
                else if (str == "}" && my_conf->get_location_block() == 1)
                {
                    my_conf->add_locations(*ok);
                    delete ok;
                    ok = new Location;
                    my_conf->set_location_block(0);
                }
                else if (str == "}" && my_conf->get_cgi_block() == 1)
                {
                    my_conf->set_cgi(*ok_cgi);
                    my_conf->add_cgi(*ok_cgi);
                    delete ok_cgi;
                    ok_cgi = new Cgi;
                    my_conf->set_cgi_block(0);
                }
                else if (str == "}" && this->server_block == 1)
                {
                    this->server_block = 0;
                    
                    std::vector<Location> Locations = my_conf->get_locations();
                    for(i=0; i < Locations.size();i++)
                    {
                        if (Locations[i].get_location_path() == "/")
                            break;
                    }
                    if (i == Locations.size())
                    {
                        Location *ok = new Location;
                        ok->set_location_path("/");
                        ok->set_allow_methods(my_conf->get_allow_methods());
                        ok->set_root(my_conf->get_root());
                        ok->set_autoindex(my_conf->get_autoindex());
                        my_conf->add_locations(*ok);
                        delete ok;
                        //
                    }
                    this->server.push_back(*my_conf);
                    delete my_conf;
                    my_conf = new Conf;
                }
                tokens.push_back(str);
            }
        }
        if (empty_line)
        {
            if (my_conf->get_location_block() == 1)
            {
                my_conf->parse_location(tokens, *ok);
            }                
            else if (my_conf->get_cgi_block() == 1)
                my_conf->parse_Cgi(tokens, *ok_cgi);
            else
                my_conf->parse_server(tokens);
            empty_line = 0;        
        }
    }
    delete ok;
    delete my_conf;
    file.close();
}

std::vector<Conf>               Servers::get_server()
{
    return this->server;
}

Servers::~Servers(void)
{
}
