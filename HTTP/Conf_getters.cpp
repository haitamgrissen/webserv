#include "Conf.hpp"

int                                     Conf::get_Port(void)
{
    return this->Port;
}

std::string                             Conf::get_host(void)
{
    return this->host;
}

std::string                             Conf::get_root(void)
{
    return this->Root;
}

std::string                             Conf::get_server_name(void)
{
    return this->server_name;
}

std::string                             Conf::get_upload_path(void)
{
    return this->upload_path;
}

int                                     Conf::get_location_count(void)
{
    return this->location_count;
}

std::string                             Conf::get_error(int c)
{
    std::map<int, std::string> errors;
    
    for (std::map<int, std::string>::iterator it = this->error_page.begin(); it != this->error_page.end(); it++)
    {
        if ((*it).first == c)
            return (*it).second;
    }
    return "";
}

std::vector <std::string>               Conf::get_allow_methods(void)
{
    for(int i = 0; i < this->Allow_methods.size(); i++)
                 std::cout << "|" << Allow_methods[i] << "|" ;
    std::cout << std::endl;
    return this->Allow_methods;
}

std::vector <std::string>               Conf::get_index(void)
{
    for(int i = 0; i < this->Index.size(); i++)
                 std::cout << "|" << this->Index[i] << "|" ;
    std::cout << std::endl;
    return this->Index;
}

std::map<int, std::string>              Conf::get_error_page(void)
{
    for (std::map<int, std::string>::iterator it = this->error_page.begin(); it != this->error_page.end(); it++)
    {
		std::cout << "\t\t\t\t" << (*it).first  << "\t" << (*it).second  <<  std::endl;
	}
    std::cout << std::endl;
    return this->error_page;
}

bool Conf::isNumber(std::string & s)
{
    for (int i=0; i < s.size();i++)
    {
        char const c = s.c_str()[i];
        if (std::isdigit(c) == 0) 
            return false;
    }
    return true;
 }

std::map<std::string, std::string>      Conf::get_redirection(void)
{
    // for (std::map<std::string, std::string>::iterator it = this->redirection.begin(); it != this->redirection.end(); it++)
    // {
	// 	std::cout << "\t\t\t\t" << (*it).first  << "\t" << (*it).second  <<  std::endl;
	// }
    // std::cout << std::endl;
    return this->redirection;
}

std::string                             Conf::get_redirection_value(std::string c)
{
    for (std::map<std::string, std::string>::iterator it = this->redirection.begin(); it != this->redirection.end(); it++)
    {
        std::cout << "\t\t\t\t" << (*it).first  << " " << c  << "|" << std::endl;
        if ((*it).first == c)
            return (*it).second;
	}
    return "";
}


int                                     Conf::get_client_max_body_size(void)
{
    return this->client_max_body_size;
}

bool                                    Conf::get_autoindex()
{
    return this->autoindex;
}

std::vector<Location>                   Conf::get_locations(void)
{
    return this->locations;
}

Cgi                                     Conf::get_cgi(void)
{
    return this->cgi;
}

int                                     Conf::get_location_block(void)
{
    return this->location_block;
}

int                                     Conf::get_cgi_block(void)
{
    return this->cgi_block;
}