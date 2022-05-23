#include "Conf.hpp"
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

void                             Conf::parse_server(std::vector <std::string> tokens)
{
    if (tokens[0] == "listen" && tokens.size() == 3)
    {
        this->host = tokens[1];
        this->Port = std::stoi(tokens[2]);
    }
    else if (tokens[0] == "root" && tokens.size() == 2)
        this->Root = tokens[1];
    else if (tokens[0] == "allow_methods")
    {
        for(int i=1; i < tokens.size();i++)
            this->Allow_methods.push_back(tokens[i]); 
    }
    else if (tokens[0] == "index")
    {
        for(int i=1; i < tokens.size();i++)
            this->Index.push_back(tokens[i]); 
    }
    else if (tokens[0] == "error_page" && tokens.size() == 3)
    {
        int i;

        i = std::stoi(tokens[1]);
        this->error_page.insert(std::make_pair(i, tokens[2]));
    }
    else if (tokens[0] == "redirection" && tokens.size() == 3)
        this->redirection.insert(std::make_pair(tokens[1], tokens[2]));
    else if (tokens[0] == "client_max_body_size" && tokens.size() == 2)
        this->client_max_body_size = std::stoi(tokens[1]);
    else if (tokens[0] == "autoindex" && tokens.size() == 2)
    {
        if (tokens[1] == "on")
            this->autoindex = true;
        else
            this->autoindex = false;
    }    
}

void                            Conf::parse_location(std::vector <std::string> tokens, Location & a)
{
    std::vector <std::string>           allow_methodss;
    std::vector <std::string>           index_it;
    std::map<int, std::string>          location_error_page;

    if (tokens[0] == "location" && tokens.size() == 2)
        this->location_count++;
    else if (tokens[0] == "path" && tokens.size() == 2)
        a.set_location_path(tokens[1]);
    else if (tokens[0] == "listen" && tokens.size() == 2)
        a.set_Listen(tokens[1]);
    else if (tokens[0] == "root" && tokens.size() == 2)
        a.set_root(tokens[1]);
    else if (tokens[0] == "allow_methods")
    {
        for(int i=1; i < tokens.size();i++)
            allow_methodss.push_back(tokens[i]); 
        a.set_allow_methods(allow_methodss);
    }
    else if (tokens[0] == "index")
    {
        for(int i=1; i < tokens.size();i++)
            index_it.push_back(tokens[i]); 
        a.set_index(index_it);
    }
    else if (tokens[0] == "error_page" && tokens.size() == 3)
    {
        int i;

        i = std::stoi(tokens[1]);
        location_error_page.insert(std::make_pair(i, tokens[2]));
        a.set_error_page(location_error_page);
    }
    else if (tokens[0] == "client_max_body_size" && tokens.size() == 2)
        a.set_client_max_body_size(std::stoi(tokens[1]));
    else if (tokens[0] == "autoindex" && tokens.size() == 2)
    {
        if (tokens[1] == "on")
            a.set_autoindex(true);
        else
            a.set_autoindex(false);
    }
}

void                            Conf::parse_Cgi(std::vector <std::string> tokens, Cgi & a)
{
    std::vector <std::string>           allow_methodss;
    std::vector <std::string>           index_it;
    std::map<int, std::string>          Cgi_error_page;

    if (tokens[0] == "cgi" && tokens.size() == 2)
        a.set_cgi_block_path(tokens[1]);
    else if (tokens[0] == "root" && tokens.size() == 2)
        a.set_root(tokens[1]);
    else if (tokens[0] == "cgi_path" && tokens.size() == 2)
        a.set_cgi_path(tokens[1]);
    else if (tokens[0] == "allow_methods")
    {
        for(int i=1; i < tokens.size();i++)
            allow_methodss.push_back(tokens[i]); 
        a.set_allow_methods(allow_methodss);
    }
    else if (tokens[0] == "index")
    {
        for(int i=1; i < tokens.size();i++)
            index_it.push_back(tokens[i]); 
        a.set_index(index_it);
    }
    else if (tokens[0] == "error_page" && tokens.size() == 3)
    {   
        int i;

        i = std::stoi(tokens[1]);
        Cgi_error_page.insert(std::make_pair(i, tokens[2]));
        a.set_error_page(Cgi_error_page);
    }
    else if (tokens[0] == "client_max_body_size" && tokens.size() == 2)
        a.set_client_max_body_size(std::stoi(tokens[1]));
    else if (tokens[0] == "autoindex" && tokens.size() == 2)
    {
        if (tokens[1] == "on")
            a.set_autoindex(true);
        else
            a.set_autoindex(false);
    }
}