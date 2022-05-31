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


Location::Location(void)
{
}

int    file_length(std::ifstream file)
{
    std::string ok;
    int i = 0;

    while(getline(file, ok))
        i++;
    file.close();
    return i;
}

std::string                             Location::get_Listen(void)
{
    return this->location_Listen;
}

std::string                             Location::get_root(void)
{

    return this->location_Root;
}

std::vector <std::string>               Location::get_allow_methods(void)
{
    // for(int i = 0; i < this->location_Allow_methods.size(); i++)
    //     std::cout << "|" << location_Allow_methods[i] << "|" ;
    // std::cout << std::endl;
    
    return this->location_Allow_methods;
}

std::vector <std::string>               Location::get_index(void)
{
    //std::cout << this->location_Index.size() << std::endl;
    return this->location_Index;
}

std::map<int, std::string>              Location::get_error_page(void)
{
    return this->location_error_page;
}

std::map<std::string, std::string>      Location::get_redirection(void)
{
    return this->location_redirection;
}

int                                     Location::get_client_max_body_size(void)
{
    return this->location_client_max_body_size;
}

bool                                    Location::get_autoindex(void)
{
    return this->location_autoindex;
}

std::string                                    Location::get_location_path(void)
{
    return this->location_path;
}

Location::~Location(void)
{
}