#ifndef Conf_hpp
#define Conf_hpp

#include <stdio.h>
#include <sys/socket.h>
# include <netinet/in.h>
#include <map>
#include <string>
#include <iterator>
#include <vector>
#include "Location.hpp"
#include "Cgi.hpp"
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

class Conf
{
    private:
        // std::string                         Listen;
        int                                 Port;
        std::string                         host;
        std::string                         Root;
        std::vector <std::string>           Allow_methods;
        std::vector <std::string>           Index;
        std::map<int, std::string>          error_page;
        std::map<std::string, std::string>  redirection;
        int                                 client_max_body_size;
        bool                                autoindex;
        std::vector<Location>               locations;
        Cgi                                 cgi;
        int                                 location_block;
        int                                 server_block;
        int                                 cgi_block;
        int                                 location_count;
        std::string                         server_name;
        std::string                         upload_path;
        

    public:
        Conf(void);
        Conf(std::string File_name);
        int                                     get_Port(void);
        int                                     get_location_count(void);
        std::string                             get_host(void);
        std::string                             get_root(void);
        std::string                             get_upload_path(void);
        std::string                             get_server_name(void);
        std::vector <std::string>               get_allow_methods(void);
        std::vector <std::string>               get_index(void);
        std::map<int, std::string>              get_error_page(void);
        std::string                             get_error(int c);
        std::map<std::string, std::string>      get_redirection(void);
        int                                     get_redirection_value(std::string c);
        int                                     get_client_max_body_size(void);
        bool                                    get_autoindex(void);
        std::vector<Location>                   get_locations(void);
        Cgi                                     get_cgi(void);
        int                                     get_location_block(void);
        int                                     get_cgi_block(void);
        void                                    parse_server(std::vector <std::string> tokens);
        void                                    parse_location(std::vector <std::string> tokens, Location & ok);
        void                                    parse_Cgi(std::vector <std::string> tokens, Cgi & ok);
        

        void                                    set_host(std::string c);
        void                                    set_Port(int c);
        void                                    set_root(std::string c);
        void                                    set_upload_path(std::string c);
        void                                    set_server_name(std::string c);
        void                                    set_allow_methods(std::vector <std::string> c);
        void                                    set_index(std::vector <std::string> c);
        void                                    set_error_page(std::map<int, std::string> c);
        void                                    set_redirection(std::map<std::string, std::string> c);
        void                                    set_client_max_body_size(int c);
        void                                    set_autoindex(bool c);
        void                                    set_locations(std::vector<Location> c);
        void                                    set_cgi(Cgi c);
        void                                    set_location_block(int c);
        void                                    set_cgi_block(int c);
        void                                    add_locations(Location c);
    
        bool                                    isNumber(std::string & s);
        void                                    show_locations();
        void                                    show_server();
        void                                    show_servers();
        void                                    show_cgi();
        ~Conf(void);
};

#endif 