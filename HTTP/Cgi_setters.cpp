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

void                                    Cgi::set_Listen(std::string c)
{
    this->Cgi_Listen = c;
}

void                                     Cgi::set_cgi_block_path(std::string c)
{
   this->cgi_block_path = c;
}

void                                     Cgi::set_root(std::string c)
{
    this->Cgi_Root = c;
}

void                                     Cgi::set_cgi_path(std::string c)
{
    this->cgi_path = c;
}

void                                     Cgi::set_allow_methods(std::vector <std::string> c)
{
   this->Cgi_Allow_methods = c;
}

void                                     Cgi::set_index(std::vector <std::string> c)
{
    this->Cgi_Index = c;
}

void                                     Cgi::set_error_page(std::map<int, std::string> c)
{
    this->Cgi_error_page  = c;
}

void                                     Cgi::set_redirection(std::map<std::string, std::string> c)
{
    this->Cgi_redirection = c;
}

void                                     Cgi::set_client_max_body_size(int c)
{
    this->Cgi_client_max_body_size = c;
}

void                                      Cgi::set_autoindex(bool c)
{
    this->Cgi_autoindex = c;
}