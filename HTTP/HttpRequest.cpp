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

std::string                 HttpRequest::get_my_upload_path(void)
{
    return this->my_upload_path;
}

void                        HttpRequest::set_my_upload_path(std::string c)
{
    this->my_upload_path = c;
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

    for(size_t i =0; i < str.size(); i++)
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
    //std::cout << this->get_my_upload_path() << std::endl;
    result_file.open(this->get_my_upload_path() + "res" + file_type, std::ios::out);
    std::cout << this->get_my_upload_path() + "res" + file_type << std::endl;
    while(getline(file_2, str))
        result_file << str + "\n";
    result_file.close();
    file_2.close();
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

int            HttpRequest::handle_chunked_body(void)
{
    std::ifstream       file_2("body.txt");
    std::fstream        result_file;
    int                 my_total = 0;
    std::string         str;
    int                 hex_flag = 0;
    int                 chunk_size = 0;
    int                 count_size = 0;

    result_file.open(this->get_my_upload_path() + "res" + this->get_file_type(), std::ios::out);
    while(getline(file_2, str))
    {
        if (!hex_flag)
        {
            chunk_size = hexadecimalToDecimal(string_toupper(str));
            my_total += chunk_size; 
            if (chunk_size == 0)
                break ;
            hex_flag = 1;
        }
        else
        {
            count_size +=  str.size() + 1;   
            
            if (count_size - 2 < chunk_size)
            {
                result_file << str + "\n";   
                //my_total -= 2;
            }
        }
        if (count_size - 2 == chunk_size)
        {
            result_file << str.substr(0, str.size() - 1);
            hex_flag = 0;
            count_size = 0;
        }
    }
    result_file.close();
    file_2.close();
    return my_total;
}

HttpRequest::HttpRequest(void)
{
    this->Http_Method = "";
    this->Request_Target = "";
    this->Protocol_Version = "";
    this->content_length = 0;
    this->tranfer_encoding = "";
    this->total_size = 0;
    this->body_ind = 0;
}

void            HttpRequest::set_body_ind(int c)
{
    this->body_ind = c;
}

void            HttpRequest::set_header(std::map<std::string, std::string> c)
{
    this->header = c;
}

size_t          HttpRequest::get_total_size(void)
{
    return this->total_size;
}

std::string     HttpRequest::get_my_host(void)
{
    return this->my_host;
}

int             HttpRequest::get_my_port(void)
{
    return this->my_port;
}

bool            HttpRequest::isNumber(std::string  s)
{
    for (size_t i=0; i < s.size();i++)
    {
        char const c = s.c_str()[i];
        if (std::isdigit(c) == 0) 
            return false;
    }
    return true;
 }

void            HttpRequest::store_header_vars(std::string req_handle, std::ostringstream & body_stream, std::string & store_it)
{
    int         pos;
    std::string result;

    pos = req_handle.find("\n");
    this->parse_line(req_handle.substr(0, pos));
    this->set_header(this->Parse_Map(req_handle.substr(pos + 1, req_handle.size())));
    this->Http_Method = this->Get_Http_Method();
    this->tranfer_encoding = this->get_value("Transfer-Encoding");
    if (this->Http_Method == "POST" )
    {
        if (this->isNumber(this->get_value("Content-Length")) && this->get_value("Content-Length") != "")    
            this->content_length = std::stoi(this->get_value("Content-Length"));
        else
            this->content_length = -1;
        //std::cout << store_it << std::endl;
        //std::cout << store_it << std::endl;
        if (store_it != "")
            store_it.erase(store_it.find_last_of("\n"));
        body_stream << store_it; 
        //std::cout << "iv reached here buddy5" << std::endl;
    }
    result = this->get_value("Host");
    this->my_host = result.substr(0, result.find(":"));
    this->my_port = stoi(result.substr(result.find(":") + 1, result.size()));
    
}

void            HttpRequest::get_request(std::string data, size_t & body_size, std::ostringstream & body_stream)
{
    std::string         req_handle;
    std::ifstream       file_2("tmp.txt");
    std::string         str;
    std::string         store_it;
    int                 flag = 0;

    req_handle = "";
    store_it = "";
    data = "";
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
    this->store_header_vars(req_handle, body_stream, store_it);
    file_2.close();
}

int           HttpRequest::store_body_content(size_t &body_size, std::ostringstream & body_stream, std::string & data, int red)
{
    body_size += red;
    //std::cout << "Y soy here " << std::endl;
    //std::cout << this->content_length << std::endl;
    //std::cout << body_size - 1 << std::endl;

    if (this->tranfer_encoding != "" && data.find("0\r\n\r\n") != std::string::npos)
    {
        //std::cout << "Condition 1 " << std::endl;
        body_stream << data << std::endl;
        return 0;
    }
    else if (this->content_length == body_size - 1)
    {
        //std::cout << "Condition 2 " << std::endl;
        body_stream << data << std::endl; 
        return 0;
    }
    else if (body_size - 1 > this->content_length && this->tranfer_encoding == "")
    {
        //std::cout << "Condition 3 " << std::endl;
        this->content_length = 0;
        return 0;
    }
    body_stream << data;
    return 1;
}

int           HttpRequest::read_data_from_fd(int & valread, std::string & data, int new_socket)
{
    char                buffer[5000] = {0}; 

    data = "";
    valread = read( new_socket , buffer, sizeof(buffer));
    if (valread <= 0)
        return 0;
    this->total_size += valread;
    for (int i = 0; i < valread; i++)
        data+=buffer[i];
    return valread;
}

int            HttpRequest::get_content_len()
{
    return this->content_length;
}

int            HttpRequest::handle_http_request(int new_socket, size_t &body_size, std::ostringstream & body_stream)
{
    std::string         data;
    std::fstream        file;
    int                 valread;

    valread = read_data_from_fd(valread, data, new_socket);
    //std::cout << "hana hna " << std::endl;
    if (valread <= 0)
    {
       // std::cout << "KKKK" << std::endl;
        file.close();
        return -1; // return -1 if read failed
    }
    if (this->Http_Method == "")
    {
        file.open("tmp.txt", std::ios::out);
        file << data;
        file.close();
        this->get_request(data, body_size, body_stream);
        //std::cout << this->Http_Method << std::endl;
        //std::cout << "iv reached here buddy 2" << std::endl;
        if (this->Http_Method != "POST")
            return 0;
        else if (body_stream.str() == "" && this->content_length != 0)
        {
            //std::cout << "??" << std::endl;
            return 0;
        }
            
        else if (valread < 5000)
        {
            //std::cout << "?????" << std::endl;
            return 0;
        }
            
    }
    else if (this->Http_Method == "POST")
    {
        //std::cout << "iv reached here buddy 3" << std::endl;
        if (!store_body_content(body_size, body_stream, data, valread))
            return 0;
    }
    else
        return 0;
    //std::cout << valread << std::endl;
    if (valread < 5000 && this->Http_Method != "POST")
        return 0;
    data.clear();
    return 1; 
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