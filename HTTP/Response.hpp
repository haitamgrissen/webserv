#ifndef Response_hpp
#define Response_hpp

#include <stdio.h>
#include <sys/socket.h>
# include <netinet/in.h>
#include <map>
#include <string>
#include <iterator>
#include <vector>
#include "Location.hpp"
#include "Cgi.hpp"
#include "Conf.hpp"
#include "Servers.hpp"

class Response
{
    private:
        std::string                     Display_type;
        int                             status_code;
        std::vector<Conf>               my_servers;      
        int                             pos;  
        std::string                     abs_path;
        std::string                     dir_list;
        std::string                     my_buffer;
        char                            *hello;
        std::ostringstream              my_Res;
        std::string                     dir_body;
        size_t                          body_size;
        size_t                          total_size;
    public:
        Response(void);
        Response(std::string File_name);
        char                        *get_date(void);
        std::string                 check_file(void);
        size_t                      handle_Get_response(void);
        void                        error_handling(std::string error);
        std::string                 get_file_type(std::string type);
        bool                        check_dir(std::string path);
        std::string                 split_file_path(std::string type);
        void                        set_mybuffer(std::string c);
        std::string                 get_mybuffer(void);
        char*                       get_hello();
        void                        set_hello(std::string c);
        std::string                 genErrorPage(int code, const std::string &msg);
        size_t                      get_body(std::string path);
        void                        handle_delete_response(std::string connection);
        void                        handle_post_response(std::string connection);
        size_t                      get_body_size(void);
        size_t                      get_total_size(void);
        ~Response(void);
};

#endif 