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
Conf::Conf(void)
{
    this->location_count = 0;
}

void                                    Conf::show_locations()
{
    std::vector<Location> ok = this->get_locations();
    for(int i = 0; i < ok.size(); i++)
    {
        std::cout << "\t\t\tLocation BLOCK\t\t\t" << std::endl;
        std::cout << "Root : \t\t\t\t" << ok[i].get_root() << std::endl;
        std::cout << "Allowed methods :\t\t";
        ok[i].get_allow_methods();
        std::cout << "Index : \t\t\t";
        ok[i].get_index();
        std::cout << "Auto index :\t\t\t" << ok[i].get_autoindex() << std::endl;
    }
}

void                                    Conf::show_cgi()
{
    Cgi example = this->get_cgi();
    std::cout << std::endl;
    std::cout << "\t\t\tCgi BLOCK\t\t\t" << std::endl;
    std::cout << "Allowed methods :\t\t";
    example.get_allow_methods();
    std::cout << "Cgi_path : \t\t\t" << example.get_cgi_path() << std::endl;
}

void                                    Conf::show_server()
{
    std::cout << "\t\t\tSERVER BLOCK\t\t\t" << std::endl;
    std::cout << std::endl;
    std::cout << "Host and Port :\t\t\t" <<this->get_host() << this->get_Port() << std::endl;
    std::cout << "Root : \t\t\t\t" << this->get_root() << std::endl;
    std::cout << "Allowed methods :\t\t";
    this->get_allow_methods();
    std::cout << "Index : \t\t\t";
    this->get_index();
    std::cout << "Error_page : " << std::endl;;
    this->get_error_page();
    std::cout << "redirection : " << std::endl;;
    this->get_redirection();
    std::cout << "client max body size :\t\t" << this->get_client_max_body_size() << std::endl;
    std::cout << "Auto index :\t\t\t" << this->get_autoindex() << std::endl;
}

void                                    Conf::show_servers()
{
    this->show_server();
    std::cout << std::endl;
    this->show_locations();
    this->show_cgi();
}

Conf::~Conf(void)
{
}
