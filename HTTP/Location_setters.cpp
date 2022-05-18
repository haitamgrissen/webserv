#include "Location.hpp"
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

void                                    Location::set_Listen(std::string c)
{
    this->location_Listen = c;
}

void                                     Location::set_root(std::string c)
{
    this->location_Root = c;
}

void                                     Location::set_allow_methods(std::vector <std::string> c)
{
   this->location_Allow_methods = c;
}

void                                     Location::set_location_path(std::string c)
{
   this->location_path = c;
}

void                                     Location::set_index(std::vector <std::string> c)
{
    this->location_Index = c;
}

void                                     Location::set_error_page(std::map<int, std::string> c)
{
    this->location_error_page  = c;
}

void                                     Location::set_redirection(std::map<std::string, std::string> c)
{
    this->location_redirection = c;
}

void                                     Location::set_client_max_body_size(int c)
{
    this->location_client_max_body_size = c;
}

void                                      Location::set_autoindex(bool c)
{
    this->location_autoindex = c;
}