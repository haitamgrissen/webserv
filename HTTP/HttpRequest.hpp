#ifndef HttpRequest_hpp
#define HttpRequest_hpp

#include <stdio.h>
#include <sys/socket.h>
# include <netinet/in.h>
#include <map>
#include <string>
#include <iterator>
#include "Location.hpp"


class HttpRequest
{
    private:
        std::string                         Http_Method;
        std::string                         Request_Target;
        std::string                         Protocol_Version;
        std::map<std::string, std::string>  header;
        int                                 fd;
        size_t                              content_length;
        std::string                         tranfer_encoding;
        size_t                              total_size;
        std::string                         my_upload_path;
        std::string                         my_host;
        int                                 my_port;
        int                                 body_ind;
        int                                 total_read;
    public:
        HttpRequest(void);
        std::map<std::string, std::string>    Parse_Map(std::string buff);
        HttpRequest(std::string buff);

        void            parse_line(std::string str);
        std::string     Get_Http_Method(void);
        std::string     Get_Request_Target(void);
        std::string     Get_Protocol_Version(void);
        int             handle_chunked_body(void);
        std::string     get_my_upload_path(void);
        void            set_my_upload_path(std::string c);
        void            handle_regular_body(void);
        int             handle_http_request(int new_socket, std::fstream & body_file, size_t &body_size, std::ostringstream & body_stream);
        void            get_request(std::string data, size_t & body_size, std::ostringstream & body_stream);
        void            list_map(void);
        std::string     get_value(std::string key);
        std::string     get_file_type(void);
        void            set_header(std::map<std::string, std::string> c);
        size_t          get_total_size(void);
        void            store_header_vars(std::string req_handle, std::ostringstream & body_stream, std::string & store_it);
        int             store_body_content(size_t &body_size, std::ostringstream & body_stream, std::string & data, int red);
        int             read_data_from_fd(int & valread, std::string & data, int new_socket);
        std::string     get_my_host(void);
        int             get_my_port(void);
        int             get_content_len();
        void            set_body_ind(int c);
        bool            isNumber(std::string  s);
        ~HttpRequest(void);
};
//body_file
#endif