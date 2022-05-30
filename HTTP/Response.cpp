#include "Conf.hpp"
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
#include "Response.hpp"
#include "Servers.hpp"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <map>
#include <cstdlib>
#include <vector>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <istream>
#include <sys/types.h>
#include <signal.h>
#include <string>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <iomanip>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
Response::Response(void)
{
    Servers ok;

    this->max_body_size = 0;
    ok.parse_server("HTTP/conf");
    this->my_servers = ok.get_server();
    this->body_size = 0;
    this->req_method = "";
    this->my_upload_path = "";
    this->regular_path = "";
    
}

void                     Response::set_request_method(std::string c)
{
    this->req_method = c;
}

std::string              Response::get_request_method(void)
{
    return this->req_method;
}

void                     Response::set_request_target(std::string c)
{
    this->req_target = c;
}

std::string              Response::get_request_target(void)
{
    return this->req_target;
}

Response::~Response(void)
{
}

char               *Response::get_date(void)
{
    time_t	now = time(0);
	char	*date = new char[30]();
	strftime(date, 29, "%a, %d %b %Y %T %Z", gmtime(&now));
    return date;
}

std::string              Response::get_error_body(std::string path)
{
    std::ostringstream	my_Res_error;
    std::string         line;
    char                buffer[30000] = {0};
    struct stat         status;
    int                 len = 0;
    int                 fd;

    fd = open(path.c_str(), O_RDONLY);
    stat(path.c_str(), &status);
    while(this->body_size < status.st_size)
    {
        len = read(fd, &buffer, 3000);
        this->body_size += len;
        for (size_t i = 0; i < len; i++)
            line += buffer[i];
        my_Res_error << line;
        line.clear();
    }
    close(fd);
    return my_Res_error.str();
}

Conf                Response::get_server(int index)
{
    return this->my_servers[index];
}

size_t              Response::get_body(std::string path)
{
    int                 len;
    char                buffer[5000] = {0};
    struct stat         status;
    int                 fd;

    len = 0;
    fd = open(path.c_str(), O_RDONLY);
    stat(path.c_str(), &status);
    while(this->body_size < status.st_size)
    {
        len = read(fd, buffer, sizeof(buffer));
        if (len <= 0)
            break;
        this->body_size += len;
        for (size_t i = 0; i < len; i++)
            this->my_Res << buffer[i];
    }
    close(fd);
    return this->body_size;
}

std::string  Response::genErrorPage(int code, const std::string &msg)
{
    std::string error_page;

    error_page = this->my_servers[_index].get_error(code);
    return error_page;
}

void                         Response::set_mybuffer(std::string c)
{
    this->my_buffer = c;
}
std::string                  Response::get_mybuffer(void)
{
    return this->my_buffer;
}

char*                       Response::get_hello()
{
    return this->hello;
}
void                        Response::set_hello(std::string c)
{
    this->hello = (char *)malloc(sizeof(char ) * c.size() + 1);
    memcpy( this->hello, c.c_str(), c.size());
}

void                        Response::set_max_body_size(int c)
{
    this->max_body_size = c;
}

int                         Response::get_max_body_size(void)
{
    return this->max_body_size;
}

void                        Response::clear()
{
    free(this->hello);
    //delete[] this->hello;
}

std::string                Response::check_file()
{
    Servers                     ok;
    std::string                 path;
    std::vector <std::string>   location_paths;
    std::string                 str;
    std::vector <std::string>   tokens;
    std::stringstream           check(path);
    struct stat                 status;

    path = this->get_mybuffer();
    this->regular_path = this->get_mybuffer();
    ok.parse_server("HTTP/conf");//TODO:change with passed argument 
    while(getline(check, str, '/'))
        tokens.push_back(str);   
    for(int i=0; i < ok.get_server()[_index].get_locations().size(); i++)
        location_paths.push_back(ok.get_server()[_index].get_locations()[i].get_location_path());
    this->max_body_size = ok.get_server()[_index].get_client_max_body_size();
    for(int i=0; i < location_paths.size() ; i++)
    {
        if (location_paths[i] == path)
        {
            this->pos = i;
            this->abs_path = ok.get_server()[_index].get_locations()[i].get_root() + path;
            return this->abs_path;
        }
        else if (location_paths[i] == path + "/")
        {
            this->pos = i;
            this->abs_path = ok.get_server()[_index].get_locations()[i].get_root() + path + "/";
            this->regular_path += "/";
            return this->abs_path;
        }
    }
    this->abs_path = ok.get_server()[_index].get_root() + path;
    stat(this->abs_path.c_str(), &status);
    if (S_ISDIR(status.st_mode))
    {
        if (this->abs_path[this->abs_path.size() - 1] != '/')
        {
            this->abs_path += "/";
            this->regular_path += "/";
        }
    }
    else
        this->abs_path = "error";
    return this->abs_path;
}

bool                        Response::check_dir(std::string path)
{
    std::ostringstream  my_html_page;
    DIR *               dir;
    struct dirent       *dir_ind;
    struct stat		    status;

    my_html_page << "<html>\n<head><title>Index of " << this->abs_path << "</title></head>\n<body>\n<h1>Index of "\
				<< this->abs_path << "</h1>\n<hr><pre><a href=\"../\">../</a>\n";
    if ((dir = opendir(path.c_str())) != NULL)
    {
        while((dir_ind = readdir(dir)) != NULL) // loops around the dir
        {
            std::string buff(dir_ind->d_name); // store the file/dir found in the path in buff
            if (buff == "." || buff == "..") // ignore .. .
                continue;
            stat((path + buff).c_str(), &status);
            if (S_ISDIR(status.st_mode)) // check if its another directory 
                buff += "/"; // add / so we can recheck on it afterwards;
            time_t	now = time(0);
	        char	*date = new char[30]();
	        strftime(date, 29, "%a, %d %b %Y %T %Z", gmtime(&now));
            my_html_page << "<a href=\"" << buff << "\">" << buff << "</a>"; 
            my_html_page << std::setw(62 - buff.length()) << date;
            if(S_ISDIR(status.st_mode))
				my_html_page << std::setw(21) << std::right << "-\n";
			else{
				my_html_page << std::setw(20) << status.st_size << std::endl;
			}
            delete[] date;
        }
        my_html_page << "</pre><hr></body>\n</html>";
        this->dir_list = my_html_page.str();
        closedir(dir);
        return true;
    }
    return false;
}

void               Response::error_handling(std::string error)
{
    std::ostringstream	my_Res_error;
    std::string         error_page;
    std::string         Error_ind;
    struct stat			status;
    time_t	now = time(0);
    char	*date = new char[30]();
    strftime(date, 29, "%a, %d %b %Y %T %Z", gmtime(&now));

    Error_ind = error.substr(0, 3);
    error_page = genErrorPage(std::stoi(Error_ind), error.substr(4, error.size()));
    my_Res_error << "HTTP/1.1 " << error << std::endl;
    my_Res_error << "Date: "<< date << std::endl;
    my_Res_error << "Server: Webserv/4.4.0" << std::endl;
    stat(error_page.c_str(), &status);
    my_Res_error << "Content-Type: text/html\r\n";
    my_Res_error << "Content-Length: " << status.st_size << "\r\n";
    my_Res_error << "Connection: Closed\r\n";
    my_Res_error << "\r\n";
    my_Res_error << this->get_error_body(error_page);
    this->set_hello(my_Res_error.str());
    this->total_size = my_Res_error.str().size();
    delete[] date;
}

std::string                 Response::get_my_upload_path(void)
{
    return this->my_upload_path;
}

void                        Response::set_my_upload_path(std::string c)
{
    this->my_upload_path = c;
}

void                        Response::handle_delete_response(std::string connection)
{
    int fd = -1;

    if (remove(this->abs_path.c_str()) < 0)
	{
		if (errno == ENOENT)
			error_handling("404 Not Found");
		else
			error_handling("403 Forbidden");
	}
    else
    {
        struct stat         status;

        time_t	now = time(0);
	    char	*date = new char[30]();
	    strftime(date, 29, "%a, %d %b %Y %T %Z", gmtime(&now));
        this->my_Res << "HTTP/1.1 204 No Content\r\n";
        this->my_Res << "Date: "<< date << "\r\n";
        this->my_Res << "Server: Webserv/4.4.0\r\n";
        this->my_Res << "Connection: " << connection  << "\r\n\r\n";
        this->set_hello(this->my_Res.str());
        delete[] date;
    }
}

void                        Response::set_redirect_path(std::string c)
{
    this->redirect_path = c;
}

std::string                 Response::get_redirect_path(void)
{
    return this->redirect_path;
}

void                        Response::handle_redirect_response(std::string c)
{
    struct stat         status;
    time_t	now = time(0);
	char	*date = new char[30]();
	strftime(date, 29, "%a, %d %b %Y %T %Z", gmtime(&now));

    this->my_Res << "HTTP/1.1 301 Moved Permanently\r\n";
    this->my_Res << "Date: "<< date << "\r\n";
    this->my_Res << "Server: Webserv/4.4.0\r\n";
    this->my_Res << "Location: " << "https://" + this->get_request_target() + "/"  << "\r\n"; 
    this->my_Res << "Connection: close" << "\r\n\r\n";
    this->total_size = this->my_Res.str().size();
    std::cout << this->my_Res.str() << std::endl;
    //std::cout << this->my_Res.str() << std::endl;
    this->set_hello(this->my_Res.str());    
    delete[] date;
}

void                        Response::handle_post_response(std::string connection)
{
    struct stat         status;
    time_t	now = time(0);
	char	*date = new char[30]();
	strftime(date, 29, "%a, %d %b %Y %T %Z", gmtime(&now));

    this->my_Res << "HTTP/1.1 201 Created\r\n";
    this->my_Res << "Date: "<< date << "\r\n";
    this->my_Res << "Server: Webserv/4.4.0\r\n";
    this->my_Res << "Location: " << this->get_my_upload_path() + "\r\n"; 
    this->my_Res << "Connection: " << connection  << "\r\n\r\n";
    // this->my_Res << "Transfer-Encoding: chunked\r\n"; 
    // this->my_Res << "0\r\n\r\n" << std::endl;
    delete[] date;
    this->total_size = this->my_Res.str().size();
    std::cout << this->my_Res.str() << std::endl;
    this->set_hello(this->my_Res.str());
}

size_t                      Response::get_body_size(void)
{
    return this->body_size;
}

size_t                      Response::get_total_size(void)
{
    return this->total_size;
}

std::string                 Response::parsing_check(std::string target_file)
{
    struct stat	              status;
    std::string               my_location_path;
    std::vector<std::string>  location_methods;
    std::string               error;

    for(int i=0; i < this->my_servers[_index].get_location_count() ; i++)
    {
        
        my_location_path = this->my_servers[_index].get_locations()[i].get_location_path();
        this->my_upload_path = this->my_servers[_index].get_upload_path();
        if ((target_file == my_location_path) || (target_file == my_location_path + "/"))
        {
            location_methods = this->my_servers[_index].get_locations()[i].get_allow_methods();
            if (std::find(location_methods.begin() , location_methods.end(), this->req_method) == location_methods.end())
                return "405 Method Not Allowed";
        }
        if (this->req_method != "POST" && stat(this->abs_path.c_str(), &status) < 0)
            return "404 File Not found";
    }
    return "";
}

int                        Response::check_errors()
{
    if (errno == ENOENT)
        error_handling("404 Not Found");
    else if (errno == EACCES)
        error_handling("403 Forbidden");
    else
        error_handling("500 Internal Server Error");
    return 0; 
}

void                        Response::initiate_response(std::string & target_file)
{
    time_t	now = time(0);
	char	*date = new char[30]();
	strftime(date, 29, "%a, %d %b %Y %T %Z", gmtime(&now));
    this->total_size = 0;
    if (this->pos <= this->my_servers[_index].get_locations().size() && this->pos >= 0)
        target_file  = this->my_servers[_index].get_locations()[this->pos].get_location_path();
    else
        target_file = "";
    this->my_Res << "HTTP/1.1 200 OK\r\n";
    this->my_Res << "Date: " << date << "\r\n";
    this->my_Res << "Server: Webserv/4.2.0\r\n";
    delete []date;
}

int                        Response::search_dir_in_locations(std::string path)
{
    std::string target;

    for(int i=0;i < this->my_servers[_index].get_locations().size() ; i++)
    {
        target = this->my_servers[_index].get_locations()[i].get_location_path();
        if (path == target)
        {
            this->pos = i;
            return 1;
        }
    }
    return 0;
}

void                       Response::Edit_path()
{
    if (this->abs_path[this->abs_path.size() - 1] != '/')
        this->abs_path += "/";
}



void                        Response::handle_file(struct stat &status)
{
    std::string     file_type;
    std::string     ok;

    file_type = get_file_type(this->abs_path);
    this->my_Res << "Content-Type: " << file_type << "\r\n";
    this->my_Res << "Content-Length: " << status.st_size << "\r\n\r\n";
    this->body_size = get_body(this->abs_path);
} 

size_t                      Response::handle_Get_response(void)
{
    int                 fd;
    struct stat         status;
    std::string         target_file;
    
    fd = -1;
    if ((fd = open(this->abs_path.c_str(), O_RDONLY)) < 0)
        return this->check_errors();

    else
    {   
        close(fd);
        initiate_response(target_file);
        stat(this->abs_path.c_str(), &status);

		if (S_ISDIR(status.st_mode))
        {
            this->dir_treatment();
            return 0;
        }

        else
            handle_file(status);
            
        this->set_hello(this->my_Res.str());
        this->total_size = this->my_Res.str().size();
    }
    return this->body_size;
}

void                         Response::setIndex(int i)
{
    _index = i;
}

int                       Response::search_index_in_location()
{
    std::string         body;
    struct stat         status;

    for(int i=0;i<this->my_servers[_index].get_locations()[this->pos].get_index().size();i++)
    {
        body = this->abs_path + this->my_servers[_index].get_locations()[this->pos].get_index()[i];
        if (stat(body.c_str(), &status) >= 0)
        {
            this->abs_path = body;
            this->handle_file(status);
            this->set_hello(this->my_Res.str());
            this->total_size = this->my_Res.str().size();
            return 1;
        }
    }
    return 0;
}

int                       Response::search_index_in_server()
{
    std::string         body;
    struct stat         status;

    for(int i=0 ; i < this->my_servers[_index].get_index().size() ; i++)
    {
        body = this->abs_path + this->my_servers[_index].get_index()[i];
        if (stat(body.c_str(), &status) >= 0)
        {
            this->abs_path = body;
            this->handle_file(status);
            this->set_hello(this->my_Res.str());
            this->total_size = this->my_Res.str().size();
            return 1;
        }
    }
    return 0;
}

void                        Response::special_dir_in_location()
{
    if (this->search_index_in_location())
        return ;
    if (!this->my_servers[_index].get_locations()[this->pos].get_autoindex())
    {
        error_handling("403 Forbidden");
        return ;
    }
    if (!check_dir(this->abs_path))
    {
        error_handling("500 Internal Server Error");
        return ;
    }
    this->my_Res << "Content-Type: text/html\r\n";
    this->my_Res << "Content-Length: " << dir_list.length() << "\r\n\r\n";
    this->my_Res << this->dir_list;
    this->set_hello(this->my_Res.str());
    this->total_size = this->my_Res.str().size();
}               

void                        Response::special_dir_in_server()
{
    if (this->search_index_in_server())
        return ;

    if (!this->my_servers[_index].get_autoindex())
    {
        error_handling("403 Forbidden");
        return ;
    }

    if (!check_dir(this->abs_path))
    {
        error_handling("500 Internal Server Error");
        return ;
    }

    this->my_Res << "Content-Type: text/html\r\n";
    this->my_Res << "Content-Length: " << dir_list.length() << "\r\n\r\n";
    this->my_Res << this->dir_list;
    this->set_hello(this->my_Res.str());
    this->total_size = this->my_Res.str().size();
}

void                        Response::special_dir_treatment()
{
    if (this->search_dir_in_locations(this->regular_path))
        this->special_dir_in_location();
    else
        this->special_dir_in_server();
}

void                        Response::dir_treatment()
{
    if (this->search_dir_in_locations(this->regular_path))
        this->special_dir_in_location();
    else
        this->special_dir_in_server();
}


std::string Response::parse_response_cgi(std::string ret)
{
    std::string fake_header;
    std::string real_header;
    std::string body;
    int size;
    int startindicator;
    int endindicator;
    int pos;
    pos = ret.find("\n\r\n");
    fake_header = ret.substr(0,pos + 1);
    body = ret.substr(pos + 1, EOF);
   // std::cout  << "|||||" << body << "||||" << std::endl;
   
    if ( (startindicator = fake_header.find("HTTP/1.1")) == -1 )
        real_header += "HTTP/1.1 200 OK\n";
    else

        real_header += fake_header.substr(startindicator,fake_header.find("\n",startindicator) + 1);
     if ( (startindicator = fake_header.find("Content-type")) == -1 )
        real_header += "Content-type:text/html\n";
    else
        real_header += fake_header.substr(startindicator,fake_header.find("\n",startindicator) + 1);

    real_header += "Server: petitwebserv\n";
    real_header += "Content-Length: " + std::to_string(body.size()) + "\n\r\n";
        return real_header + body;
}