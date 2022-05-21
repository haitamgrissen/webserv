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
Cgi::Cgi(void)
{
}

std::string                             Cgi::get_Listen(void)
{
    return this->Cgi_Listen;
}

std::string                             Cgi::get_root(void)
{

    return this->Cgi_Root;
}

std::string                             Cgi::get_cgi_path(void)
{
    return this->cgi_path;
}

std::string                             Cgi::get_cgi_block_path(void)
{
    return this->cgi_block_path;
}

std::vector <std::string>               Cgi::get_allow_methods(void)
{
    for(int i = 0; i < this->Cgi_Allow_methods.size(); i++)
        std::cout << "|" << Cgi_Allow_methods[i] << "|" ;
    std::cout << std::endl;
    return this->Cgi_Allow_methods;
}

std::vector <std::string>               Cgi::get_index(void)
{
    for(int i = 0; i < this->Cgi_Index.size(); i++)
                 std::cout << "|" << this->Cgi_Index[i] << "|" ;
    std::cout << std::endl;
    return this->Cgi_Index;
}

std::map<int, std::string>              Cgi::get_error_page(void)
{
    return this->Cgi_error_page;
}

std::map<std::string, std::string>      Cgi::get_redirection(void)
{
    return this->Cgi_redirection;
}

int                                     Cgi::get_client_max_body_size(void)
{
    return this->Cgi_client_max_body_size;
}

bool                                    Cgi::get_autoindex(void)
{
    return this->Cgi_autoindex;
}

Cgi::~Cgi(void)
{
}