#ifndef Cgi_hpp
#define Cgi_hpp

#include <stdio.h>
#include <sys/socket.h>
# include <netinet/in.h>
#include <map>
#include <string>
#include <iterator>
#include <vector>

class Cgi
{
    private:
        std::string                         Cgi_Listen;
        std::string                         Cgi_Root;
        std::vector <std::string>           Cgi_Allow_methods;
        std::vector <std::string>           Cgi_Index;
        std::map<int, std::string>          Cgi_error_page;
        std::map<std::string, std::string>  Cgi_redirection;
        int                                 Cgi_client_max_body_size;
        bool                                Cgi_autoindex;
        std::string                         cgi_path;
        
    public:
        Cgi(void);
        std::string                             get_Listen(void);
        std::string                             get_root(void);
        std::vector <std::string>               get_allow_methods(void);
        std::vector <std::string>               get_index(void);
        std::map<int, std::string>              get_error_page(void);
        std::map<std::string, std::string>      get_redirection(void);
        int                                     get_client_max_body_size(void);
        std::string                             get_cgi_path(void);
        bool                                    get_autoindex();
        void                                    set_Listen(std::string c);
        void                                    set_root(std::string c);
        void                                    set_cgi_path(std::string c);
        void                                    set_allow_methods(std::vector <std::string> c);
        void                                    set_index(std::vector <std::string> c);
        void                                    set_error_page(std::map<int, std::string> c);
        void                                    set_redirection(std::map<std::string, std::string> c);
        void                                    set_client_max_body_size(int c);
        void                                    set_autoindex(bool c);
        ~Cgi(void);
};

#endif 