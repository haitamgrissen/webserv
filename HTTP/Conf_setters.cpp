#include "Conf.hpp"


void                                    Conf::set_Port(int c)
{
    this->Port = c;
}

void                                    Conf::set_host(std::string c)
{
    this->host = c;
}

void                                    Conf::set_root(std::string c)
{
    this->Root = c;
}

void                                    Conf::set_upload_path(std::string c)
{
    this->upload_path = c;
}

void                                    Conf::set_server_name(std::string c)
{
    this->server_name = c;
}

void                                    Conf::set_allow_methods(std::vector <std::string> c)
{
    this->Allow_methods = c;
}

void                                    Conf::set_index(std::vector <std::string> c)
{
    this->Index = c;
}

void                                    Conf::set_error_page(std::map<int, std::string> c)
{
    this->error_page = c;
}

void                                    Conf::set_redirection(std::map<std::string, std::string> c)
{
    this->redirection = c;
}

void                                    Conf::set_client_max_body_size(int c)
{
    this->client_max_body_size = c;
}

void                                    Conf::set_autoindex(bool c)
{
    this->autoindex = c;
}

void                                    Conf::set_locations(std::vector<Location> c)
{
    this->locations = c;
}

void                                    Conf::add_locations(Location c)
{
    this->locations.push_back(c);
}

void                                    Conf::set_cgi(Cgi c)
{
    this->cgi = c;
}

void                                    Conf::set_location_block(int c)
{
    this->location_block = c;
}

void                                    Conf::set_cgi_block(int c)
{
    this->cgi_block = c;
}
