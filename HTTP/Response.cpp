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
    Servers ok("HTTP/conf");
    this->my_servers = ok.get_server();
    this->body_size = 0;
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

size_t              Response::get_body(std::string path)
{
    std::string     ok;
    std::string     bodygen;
    std::ifstream   file(path);
    std::string     line;
    //std::fstream    body_file;
    int             len = 0;
    std::ostringstream body_stream;
    char buffer[30000] = {0};

    int ind;
    struct stat         status;

    //body_file.open("my_body.txt", std::ios::out);
    int fd = open(path.c_str(), O_RDONLY);
    stat(path.c_str(), &status);
    while(this->body_size < status.st_size)
    {
        len = read(fd, &buffer, 3000);
        this->body_size += len;
        for (size_t i = 0; i < len; i++)
            line += buffer[i];
        body_stream << line;
        line.clear();
    }
    this->my_Res << body_stream.str();
    //body_file << body_stream.str();
    return this->body_size;
}

std::string  Response::genErrorPage(int code, const std::string &msg)
{
    std::string error_page;

    error_page = this->my_servers[0].get_error(code);
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
    this->hello = (char *)malloc(sizeof(char ) * c.size());
    memcpy( this->hello, c.c_str(), c.size()); 
}

std::string                Response::check_file(void)
{
    Servers ok("HTTP/conf");
    std::string path = this->get_mybuffer();
    std::vector <std::string> location_paths;
    std::string               str;
    std::vector <std::string> tokens;
    std::stringstream         check(path);

    while(getline(check, str, '/'))
        tokens.push_back(str);   
    for(int i=0; i < ok.get_server()[0].get_locations().size(); i++)
        location_paths.push_back(ok.get_server()[0].get_locations()[i].get_location_path());

    for(int i=0; i < location_paths.size() ; i++)
    {
        if (location_paths[i] == path)
        {
            this->pos = i;
            this->abs_path = ok.get_server()[0].get_locations()[i].get_root() + path;
            std::cout << this->abs_path << std::endl;
            break;
        }            
    }
    return this->abs_path;
}


bool                        Response::check_dir(std::string path)
{
    std::ostringstream my_html_page;
    DIR *              dir;
    struct dirent      *dir_ind;
    struct stat		status;

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
            my_html_page << "<a href=\"" << buff << "\">" << buff << "</a>"; 
            my_html_page << std::setw(62 - buff.length()) << this->get_date();
            if(S_ISDIR(status.st_mode))
				my_html_page << std::setw(21) << std::right << "-\n";
			else{
				my_html_page << std::setw(20) << status.st_size << std::endl;
			}
            //std::cout << buff << std::endl;
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
    std::string status_code;
    std::ostringstream	my_Res_error;
    std::string  error_page;
    std::string         Error_ind;
    struct stat			status;

    Error_ind = error.substr(0, 3);
    error_page = genErrorPage(std::stoi(Error_ind), error.substr(4, error.size()));
    //std::cout << error_page << "|my error" << std::endl;
    my_Res_error << "HTTP/1.1 " << error << std::endl;
    my_Res_error << "Date: "<< this->get_date() << std::endl;
    my_Res_error << "Server: Webserv/4.0.4" << std::endl;

    if (Error_ind == "405") //rj3ha 405
    {
        std::vector<std::string>  c;
        std::string method;
        Servers ok("conf");
        c = ok.get_server()[0].get_locations()[this->pos].get_allow_methods();
        if (std::find(c.begin(), c.end(), "GET") != c.end())
            this->my_Res << "Allow: " << method << "\r\n";
    }
    stat(error_page.c_str(), &status);

    my_Res_error << "Content-Type: text/html\r\n";
    my_Res_error << "Content-Length: " << status.st_size << "\r\n";
    my_Res_error << "Connection: Closed\r\n";
    my_Res_error << "\r\n";
    my_Res_error << this->get_body(error_page);
    this->set_hello(my_Res_error.str());
    this->total_size = this->my_Res.str().size();
}

void                        Response::handle_delete_response(std::string connection)
{
    int fd = -1;

    check_file();
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

        this->my_Res << "HTTP/1.1 204 No Content\r\n";
        this->my_Res << "   Date: "<< this->get_date() << "\r\n";
        this->my_Res << "Server: Webserv/4.2.0\r\n";
        this->my_Res <<"Connection: " << connection  << "\r\n\r\n";
        this->set_hello(this->my_Res.str());
    }
}

void                        Response::handle_post_response(std::string connection)
{
    check_file();
    struct stat         status;

    this->my_Res << "HTTP/1.1 201 CREATED\r\n";
    this->my_Res << "Date: "<< this->get_date() << "\r\n";
    this->my_Res << "Server: Webserv/4.2.0\r\n";
    this->my_Res <<"Connection: " << connection  << "\r\n\r\n";
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

size_t                      Response::handle_Get_response(void)
{
    //std::fstream    body_file;
    int fd = -1;

    
    check_file();

    //body_file.open("my_body.txt", std::ios::out);
    if ((fd = open(this->abs_path.c_str(), O_RDONLY)) < 0)
    {
        if (errno == ENOENT)
			error_handling("404 Not Found");
		else if (errno == EACCES)
			error_handling("403 Forbidden");
		else
			error_handling("500 Internal Server Error");
         return 0;
    }
    else
    {

        std::string         body;
        std::string         Error_ind;
        struct stat         status;

        std::string target_file;
        
        target_file  = this->my_servers[0].get_locations()[this->pos].get_location_path();
        this->my_Res << "HTTP/1.1 200 OK\r\n";
        this->my_Res << "Date: "<< this->get_date() << "\r\n";
        this->my_Res << "Server: Webserv/4.2.0\r\n";
        stat(this->abs_path.c_str(), &status);
        this->total_size = 0;
		if (S_ISDIR(status.st_mode))
        {
            if (target_file != "/")  
            {
                if (std::count(target_file.begin(), target_file.end(), '/') > 1)
                {
                    if (!this->my_servers[0].get_locations()[this->pos].get_autoindex())
                    {
                        error_handling("403 Forbidden");
                        return 0;
                    }
                    if (!check_dir(this->abs_path))
                    {
                        error_handling("500 Internal Server Error");
                        return 0;
                    }
                    this->my_Res << "Content-Type: text/html\r\n";
					this->my_Res << "Content-Length: " << dir_list.length() << "\r\n\r\n";
                    this->my_Res << this->dir_list;
                    this->set_hello(this->my_Res.str());
                    this->total_size = this->my_Res.str().size();
                    return 0;
                }
                else
                {
                    error_handling("404 not found");
                    this->set_hello(this->my_Res.str());
                    this->total_size = this->my_Res.str().size();
                    return 0;
                }
            }
            else if (this->my_servers[0].get_locations()[this->pos].get_location_path() == "/")
            {
                body = this->my_servers[0].get_locations()[this->pos].get_root() + "/" + this->my_servers[0].get_index()[1];
                if (stat(body.c_str(), &status) < 0)
                {
                    error_handling("403 Forbidden");
                    return 0;
                }
                else
                {
                    this->my_Res << "Content-Type: text/html\r\n";
                    this->my_Res << "Content-Length: " << status.st_size << "\r\n";
                }
            }
            else
            {
                if (!this->my_servers[0].get_locations()[this->pos].get_autoindex())
                {
                    error_handling("403 Forbidden");
                    return 0;
                }
                if (!check_dir(this->abs_path))
				{
					error_handling("500 Internal Server Error");
					return 0;
				}
                this->my_Res << "Content-Type: text/html\r\n";
				this->my_Res << "Content-Length: " << dir_list.length() << "\r\n";
				body = "";
            }
        }
        else
        {
            std::string file_type;
            std::string ok;

            file_type = get_file_type(this->abs_path);
            //std::cout << this->abs_path << std::endl;
            this->my_Res << "Content-Type: " << file_type << "\r\n";
            this->my_Res << "Content-Length: " << status.st_size << "\r\n\r\n";
            
           // exit(0);
            this->body_size = get_body(this->abs_path);
            
        }        
        //this->my_Res << body;
        //std::cout << body << std::endl;
        //
        //exit(0);
        this->set_hello(this->my_Res.str());
        this->total_size = this->my_Res.str().size();
        std::cout << this->my_Res.str() << std::endl;
        // std::cout << "HERE " << std::endl;
        // exit(0);
        //std::cout << "My size: " << this->my_Res.str().size() << std::endl;
    }
    return this->body_size;
}

// int main()
// {
//                     // int fd = -1;
//                     // Response o;
//                     // std::string     status;
                    
//                     // Servers ok("conf");
//                     // std::cout <<  "HOST IS :"  << ok.get_server()[0].get_host() << std::endl;
//                     // std::cout << "PORT IS :" << ok.get_server()[0].get_Port() << std::endl;
//                                     Response ok;

//                                     ok.handle_response();
//                     // if ((fd = open("/errorsPages/404.html", O_RDONLY)) < 1)
//                     // {
//                     //     if (errno == ENOENT)
//                     //         status = "404 Not found";
//                     //     else if (errno == EACCES)
//                     //         status = "403 Forbidden";
//                     //     else
//                     //         status = "500 Internal Server Error";
//                     // }
//                     // else
//                     //     status = "200 OK";
//                     // //printf("%d %d\n", errno, ENOENT);
//                     // o.error_handling("404 not found");
//     // Servers ok("conf");
//     // std::string path = "/PATH03/list/ok.html";
//     // std::vector <std::string> location_paths;
    

//     // std::string root_path = "/Desktop/web_server";

//     // std::string               str;
//     // std::vector <std::string> tokens;
//     // std::stringstream         check(path);

//     // std::string last;

//     // while(getline(check, str, '/'))
//     //     tokens.push_back(str);
    
//     // for(int i=0; i < ok.get_server()[0].get_locations().size(); i++)
//     //     location_paths.push_back(ok.get_server()[0].get_locations()[i].get_location_path());

//     // for(int i=0; i < location_paths.size() ; i++)
//     // {
//     //     std::cout << location_paths[i] << "----" << tokens[1] << std::endl;
//     //     if (location_paths[i] == ("/" + tokens[1]))
//     //         last = ok.get_server()[0].get_locations()[i].get_root() + "/" + tokens[tokens.size() - 1];
//     // }

//    // std::string location_path = "/PATH03";

//     //std::string last = "/Users/nainhaja/" + root_path + "/" + tokens[tokens.size() - 1];

//                 // int fd = open(last.c_str(), O_RDONLY);
//                 // std::cout << "|" << last << "|" << std::endl;
//                 // if (fd < 0)
//                 //     std::cout << "404 not found" << std::endl;
//                 // else
//                 //     std::cout << "200 ok" << std::endl;
//     // std::string  body = genErrorPage(404, "not found");

//     // std::cout << "HTTP/1.1 " << status << std::endl;
//     // std::cout << "Date: "<< o.get_date() << std::endl;
//     // std::cout << "Server: Webserv/4.2.0" << std::endl;
//     // std::cout << "Content-Type: text/html" << std::endl;
//     // std::cout << "Content-Length: " << body.size() << std::endl;
//     // std::cout << "Connection: Closed" << std::endl;
//     // std::cout << std::endl;
//     // std::cout << body;

    
// }

// /*
//     GET
//         -() /index.html > read -> string  ) -> body 
//         -> headers 
        
//     POST
//     DELETE

// */