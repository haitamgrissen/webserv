#include "HttpRequest.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

# include <iostream>
# include <unistd.h>
# include <sstream>
# include <fstream>
# include <string>
# include <vector>
# include <map>
# include <poll.h>
# include <algorithm>
# include <sys/stat.h>
# include <fcntl.h>

#include <iostream>
#include <iomanip>
#include <sstream>

std::map<std::string, std::string> HttpRequest::Parse_Map(std::string buff)
{
    std::string                 str;
    std::vector <std::string>   tokens;
    std::string                 key;
    std::string                 value;
    std::map<std::string, std::string > my_map;
    
    std::stringstream check(buff);

    while(getline(check, str, '\n'))
    {
        key = str.substr(0, str.find(":"));
        value = str.substr(str.find(" ") + 1, str.size());
        value.erase(value.find_last_of('\r'));
        my_map.insert(std::make_pair(key, value));
    }
    return my_map;
}

int hexadecimalToDecimal(std::string hexVal)
{
    int len = hexVal.size();
 
    // Initializing base value to 1, i.e 16^0
    int base = 1;
 
    int dec_val = 0;
 
    // Extracting characters as digits from last
    // character
    for (int i = len - 1; i >= 0; i--) {
        // if character lies in '0'-'9', converting
        // it to integral 0-9 by subtracting 48 from
        // ASCII value
        if (hexVal[i] >= '0' && hexVal[i] <= '9') {
            dec_val += (int(hexVal[i]) - 48) * base;
 
            // incrementing base by power
            base = base * 16;
        }
 
        // if character lies in 'A'-'F' , converting
        // it to integral 10 - 15 by subtracting 55
        // from ASCII value
        else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') {
            dec_val += (int(hexVal[i]) - 55) * base;
 
            // incrementing base by power
            base = base * 16;
        }
    }
    return dec_val;
}

std::string string_toupper(std::string str)
{
    std::string result;
    int c;

    for(int i =0; i < str.size(); i++)
    {
        c = str[i];
        if (isalpha(str[i]))
            c = std::toupper(c);
        result += c;
    }
    return result;
}

void            HttpRequest::parse_line(std::string buff)
{
    std::string     str;
    std::vector <std::string> tokens;
    std::stringstream check(buff);

    while(getline(check, str, ' '))
        tokens.push_back(str);
    this->Http_Method = tokens[0];
    this->Request_Target = tokens[1];
    this->Protocol_Version = tokens[2];
}

void            HttpRequest::handle_regular_body(void)
{
    std::ifstream   file_2("body.txt");
    std::fstream    result_file;
    std::string     str;
    std::string     file_type;

    file_type = this->get_file_type();
    result_file.open("res" + file_type, std::ios::out);
    while(getline(file_2, str))
        result_file << str + "\n";   
}

std::string     HttpRequest::get_file_type(void)
{
    std::string type;
    std::string file_type;

    type = this->get_value("Content-Type");
    file_type = type.substr(type.find("/") + 1, type.size());
    if (file_type == "plain")
        return "";
    else
        return "." + file_type;
}

void            HttpRequest::handle_chunked_body(void)
{
    std::ifstream file_2("body.txt");
    std::fstream    result_file;
    std::string     str;
    std::string     file_type;
    int             body_flag = 0;
    int hex_flag = 0;
    int chunk_size = 0;
    int count_size = 0;
    std::string result = "";

    std::fstream    out_file;
    file_type =  this->get_file_type();
    out_file.open("my_res", std::ios::out);
    result_file.open("res" + file_type, std::ios::out);
    while(getline(file_2, str))
    {
        if (!hex_flag)
        {
            chunk_size = hexadecimalToDecimal(string_toupper(str));
            if (chunk_size == 0)
                break ;
            hex_flag = 1;
        }
        else
        {
            count_size +=  str.size() + 1;   
            if (count_size - 2 < chunk_size)
                result_file << str + "\n";   
        }
        if (count_size - 2 == chunk_size)
        {
            result_file << str.substr(0, str.size() - 1);
            hex_flag = 0;
            count_size = 0;
        }
    }
}

HttpRequest::HttpRequest(void)
{
    this->Http_Method = "";
    this->Request_Target = "";
    this->Protocol_Version = "";
    this->content_length = 0;
    this->tranfer_encoding = "";
    this->total_size = 0;
}

void            HttpRequest::set_header(std::map<std::string, std::string> c)
{
    this->header = c;
}

HttpRequest::HttpRequest(std::string buff)
{
    int pos;
    pos = buff.find("\n");
    parse_line(buff.substr(0, pos));
    
    this->header = Parse_Map(buff.substr(pos + 1, buff.size()));
    if (this->Http_Method == "POST" && this->get_value("Transfer-Encoding") == "chunked")
        handle_chunked_body();
    else if (this->Http_Method == "POST" && this->get_value("Transfer-Encoding") == "")
        handle_regular_body(); 
}

size_t          HttpRequest::get_total_size(void)
{
    return this->total_size;
}

void            HttpRequest::get_request(std::string data, size_t & body_size, std::ostringstream & body_stream)
{
    std::fstream    file;

    file.open("tmp.txt", std::ios::out);
    file << data;
    file.close();
    std::string req_handle = "";
    std::string body_handle = "";
    std::ifstream file_2("tmp.txt");
    std::string     str;
    std::string store_it = "";
    int pos;
    int flag = 0;
    while(getline(file_2, str))
    {
        if (str == "\r")
            flag = 1;
        else
        {
            if (!flag)
                req_handle += str + "\n";
            else
            {
                body_size += str.size() + 1;
                store_it += str + "\n";
            }
        }
    }
    pos = req_handle.find("\n");
    this->parse_line(req_handle.substr(0, pos));
    this->set_header(this->Parse_Map(req_handle.substr(pos + 1, req_handle.size())));
    this->Http_Method = this->Get_Http_Method();
    this->tranfer_encoding = this->get_value("Transfer-Encoding");
    if (this->Http_Method == "POST")
    {
        this->content_length = std::stoi(this->get_value("Content-Length"));
        store_it.erase(store_it.find_last_of("\n"));
        body_stream << store_it;
        
        file_2.close();        
    }
}

int            HttpRequest::handle_http_request(int new_socket, std::fstream & body_file, size_t &body_size, std::ostringstream & body_stream)
{
   //std::fstream    body_filee;
    //size_t          body_size;
    std::string     data;
    int             valread;
    char            buffer[30000] = {0};
    //std::ostringstream body_stream;

    data = "";
    //body_size = 0;
    //body_filee.open("bodyy.txt", std::ios::out);
    // while(1)
    // {
        valread = read( new_socket , buffer, 3000);
        this->total_size += valread;
        for (size_t i = 0; i < valread; i++)
            data+=buffer[i];
        if (this->Http_Method == "")
        {
            this->get_request(data, body_size, body_stream);
            if (this->Http_Method != "POST")
                return 0;
        }
        else if (this->Http_Method == "POST")
        {     
            //std::cout << " here " << std::endl;
            body_size += valread;
            if (this->tranfer_encoding != "" && data.find("0\r\n\r\n") != std::string::npos)
            {
                body_stream << data << std::endl;
                //body_filee << body_stream.str() << std::endl;
                 //std::cout << "landed here " << std::endl;
                return 0;
            }
            else if (this->content_length == body_size - 1)
            {
                body_stream << data << std::endl; 
                
                return 0 ;
            }
            body_stream << data;  
        }
        else
            return 0;
        data.clear(); 
        return 1;
    //}
    // if (this->Http_Method == "POST")
    // {
    //     body_file << body_stream.str() << std::endl;
    //     body_file.close();        
    // }
}

std::string HttpRequest::Get_Http_Method(void)
{
    return this->Http_Method;
}

std::string HttpRequest::Get_Request_Target(void)
{
    return this->Request_Target;
}

std::string HttpRequest::Get_Protocol_Version(void)
{
    return this->Protocol_Version;
}

std::string HttpRequest::get_value(std::string key)
{
    std::map<std::string, std::string>::iterator it = this->header.find(key);
    if (it != this->header.end())
        return (*it).second;
    else
        return "";
}

void    HttpRequest::list_map(void)
{
    for (std::map<std::string, std::string>::iterator it = this->header.begin(); it != this->header.end(); it++) {

		std::cout << "Key :" << (*it).first <<  std::endl;
        std::cout << "Value: " << (*it).second <<  std::endl;
	}
}

HttpRequest::~HttpRequest(void)
{
}