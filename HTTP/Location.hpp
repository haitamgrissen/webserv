#ifndef Location_hpp
#define Location_hpp

#include <stdio.h>
#include <sys/socket.h>
# include <netinet/in.h>
#include <map>
#include <string>
#include <iterator>
#include <vector>

class Location
{
    private:
        std::string                         location_Listen;
        std::string                         location_Root;
        std::vector <std::string>           location_Allow_methods;
        std::vector <std::string>           location_Index;
        std::map<int, std::string>          location_error_page;
        std::map<std::string, std::string>  location_redirection;
        int                                 location_client_max_body_size;
        bool                                location_autoindex;
        std::string                         location_path;
    public:
        Location(void);
        std::string                             get_Listen(void);
        std::string                             get_root(void);
        std::vector <std::string>               get_allow_methods(void);
        std::vector <std::string>               get_index(void);
        std::map<int, std::string>              get_error_page(void);
        std::map<std::string, std::string>      get_redirection(void);
        int                                     get_client_max_body_size(void);
        bool                                    get_autoindex();
        std::string                             get_location_path(void);

        void                                    set_Listen(std::string c);
        void                                    set_root(std::string c);
        void                                    set_allow_methods(std::vector <std::string> c);
        void                                    set_index(std::vector <std::string> c);
        void                                    set_error_page(std::map<int, std::string> c);
        void                                    set_redirection(std::map<std::string, std::string> c);
        void                                    set_client_max_body_size(int c);
        void                                    set_autoindex(bool c);
        void                                    set_location_path(std::string c);
        ~Location(void);
};

#endif 