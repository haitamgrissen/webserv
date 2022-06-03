#include "Server.hpp"
#include <signal.h>


/*
** --------------------------------- METHODS ----------------------------------
*/



int Server::Create()
{


	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
		throw SocketException();

	fcntl(_fd, F_SETFL, O_NONBLOCK);

	int enable = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	std::cout << "setsockopt(SO_REUSEADDR) failed" << std::endl;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
    	std::cout << "setsockopt(SO_REUSEPORT) failed" << std::endl;

	// linger didnt work!!!!!
	// linger lin;
	// lin.l_onoff = 1;
	// lin.l_linger = 0;
	// setsockopt(_fd, SOL_SOCKET, SO_LINGER, (const char *)&lin, sizeof(int));

	if (bind(_fd, (struct sockaddr * ) &_addr, sizeof(_addr)) == -1)
		throw BindException();
	
    
	if (listen(_fd, 10000) < 0)
		throw ListenException();
	std::cout <<  "SERVER FD : " << _fd << std::endl;

	return (0);
}

/*
** --------------------------------- I/O NETWORKING ---------------------------------
*/

int		Server::accept()
{
	int clnt;
	if ((clnt = ::accept(_fd, (struct sockaddr *)&_addr, (socklen_t*)&_addrlen)) < 0)
    {
		std::cout << "[   ]" <<  _fd << "[   ]"<< "hello from accept" << std::endl;
        return (-1);
    }
	else
	{
		fcntl(clnt, F_SETFL, O_NONBLOCK);
		_requestmap.insert(std::make_pair(clnt, new _body(_fd)));
		return (clnt);
	}
}

// int		Server::send(int sock)
// {
// 	std::map<int, _body *>::iterator	it;

// 	it = _requestmap.find(sock);
// 	if (it == _requestmap.end())
// 	{
// 		std::cout << "ERROR IN Request MAP\n";
// 		exit(EXIT_FAILURE);
// 	}


// 	_body *bd;

// 	bd = it->second;
	
	

// 	if (bd->_http.Get_Http_Method() == "POST")
// 	{
// 		bd->_body_file << bd->_body_stream.str() << std::endl;
// 		bd->_body_file.close();        
// 	}






// 	if (it->second->_startedwrite == false)
// 	{
// 		it->second->_startedwrite = true;
// 	}



// 	bd->_ok.setIndex(_index);
// 	std::string	error_msg;
// 	bd->_ok.set_request_method(bd->_http.Get_Http_Method());
// 	bd->_ok.set_request_target(bd->_http.Get_Request_Target());


// 	bd->_ok.set_mybuffer(bd->_http.Get_Request_Target());

// 	bd->_ok.check_file();
	
// 	error_msg = bd->_ok.parsing_check();
// 	///bd->_ok.error_handling(error_msg);
// 	std::cout << "?????????" << std::endl;
// 	bd->_http.set_my_upload_path(bd->_ok.get_my_upload_path());


// 	if (bd->_http.Get_Http_Method() == "POST" && bd->_http.get_value("Transfer-Encoding") == "chunked")
// 		bd->_http.handle_chunked_body();
// 	else if (bd->_http.Get_Http_Method() == "POST")
// 		bd->_http.handle_regular_body();
// 	if (bd->_ok.get_max_body_size() < 0)
// 	{
// 		bd->_ok.error_handling("500 Webservice currently unavailable");
// 	}
// 	else if (bd->_http.get_total_size() > bd->_ok.get_max_body_size() && bd->_ok.get_max_body_size() != 0)
// 		bd->_ok.error_handling("413 Payload Too Large");
// 	else
// 	{
// 		if (error_msg != "")
// 			bd->_ok.error_handling(error_msg);
// 		else
// 		{
// 			if (bd->_http.Get_Http_Method() == "GET")
// 				bd->_body_size = bd->_ok.handle_Get_response();
// 			else if (bd->_http.Get_Http_Method() == "DELETE")
// 				bd->_ok.handle_delete_response(bd->_http.get_value("Connection"));
// 			else if (bd->_http.Get_Http_Method() == "POST")
// 				bd->_ok.handle_post_response(bd->_http.get_value("Connection"));
// 		}		
// 	}	

// 	int flag;
// 	flag = write(sock , bd->_ok.get_hello() + bd->_writecount , bd->_ok.get_total_size() - bd->_writecount);
// 	bd->_writecount += flag;


// 	if (flag == 0)
// 	{
// 		_requestmap.erase(it);
// 		return 0;
// 	}
// 	if (bd->_ok.get_total_size() <= bd->_writecount)
// 	{
// 		_requestmap.erase(it);
// 		return 0;
// 	}
// 	else
// 		return (1);

// }



int		Server::send(int sock, _body * bd, std::string config)
{
	std::string	my_method;
	std::string	my_chunk;
	int			my_len;
	std::string	error_msg = "";
	std::string	request_target;
	std::string	red_target;

	bd->_ok.arrange_config(config);
	bd->init_values(my_method, my_chunk, _index, my_len, request_target);
	
	if (bd->_startedwrite == false)
		bd->_startedwrite = true;

	int cgi = CGI_D_ayoub(bd, bd->_http.Get_Request_Target(), bd->_http.Get_Http_Method());
	//int  cgi = 0;
	//std::cout << "???????" << std::endl;
	if (cgi == 0)
	{
		error_msg = bd->_ok.check_my_location(bd->_http.Get_Request_Target(), bd->_http.Get_Http_Method());
		
		red_target = request_target.substr(request_target.find_last_of("/") + 1, request_target.size());
		red_target = bd->_ok.get_server(_index).get_redirection_value(red_target);
		
		//error_msg = bd->_ok.pars_check(bd->_http.Get_Request_Target(), bd->_http.Get_Http_Method());
		if (error_msg != "")
		{
			bd->_ok.error_handling("405 Method Not Allowed");
		}
		else if (red_target != "")
		{
			bd->_ok.set_request_target(red_target);
			bd->_ok.handle_redirect_response(bd->_http.get_value("Connection"));
		}
		else if (my_method == "POST" && ((my_len < 0) || (bd->_body_stream.str() == "" && my_len != 0)))
			bd->_ok.error_handling("400 Bad Request");
		else
		{
			if (!bd->handle_body(my_method, my_chunk, error_msg, my_len))
				bd->_ok.error_handling("400 Bad Request");
			else
			{
				if (bd->_ok.get_max_body_size() < 0)
					bd->_ok.error_handling("500 Webservice currently unavailable");
				else if (bd->_http.get_total_size() > (size_t)bd->_ok.get_max_body_size() && bd->_ok.get_max_body_size() != 0)
					bd->_ok.error_handling("413 Payload Too Large");
				else
				{
					if (error_msg != "")
						bd->_ok.error_handling(error_msg);
					else
						bd->handle_response(my_method);
					
				}			
			}
		}
	}
	else if (cgi == -1)///executable scripte doesn exist 
	{	
		//std::cout << "404 Not Found" << std::endl;
		bd->_ok.error_handling("404 Not Found");
		cgi = 0;
		//bd->_ok.error_handling("400 Bad Request");
		///error  not found exec file 404
		//return 0;
	}
	else if (cgi == -2)
	{	
		bd->_ok.error_handling("500 Webservice currently unavailable");
		///internal SERVER ERROR 500
		//return 0;
		cgi = 0;
	}
	else if (cgi == -3)//time out 
	{	
		bd->_ok.error_handling("504 Gateway Timeout");
		///internal SERVER ERROR 500
		//return 0;
		cgi = 0;
	}








	///Send the handeled Respones
	int flag;
	if (cgi == 0)
	{
		flag = write(sock , bd->_ok.get_hello() + bd->_writecount , bd->_ok.get_total_size() - bd->_writecount);
	}
		
	else
		flag = write(sock, bd->response.c_str() + bd->_writecount , bd->response.size() - bd->_writecount);




	bd->_writecount += flag;
	bd->_ok.clear();
	bd->close_file();
	//delete[] bd;
	if (bd->_ok.get_total_size() <= bd->_writecount)
	{
		// _requestmap.erase(it);
		return 0;
	}
	else if (flag == -1 || flag == 0) 
		return (0);
	else
		return 1;
	

}


int		Server::recv(int sock)
{
	std::map<int, _body *>::iterator	it;
	int									flag;

	it = _requestmap.find(sock);
	if (it ==  _requestmap.end())
	{
		std::cout << "couldnt receve request\n";
		return (-1);//TODO:ghir t9mira 
	}
	else
	{
		_body *bd = it->second;
		flag = bd->_http.handle_http_request(sock, bd->_body_size, bd->_body_stream);
		return flag;
	}
}

/*
** --------------------------------- Modefiers ---------------------------------
*/


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

	void 	Server::setSocket(std::string hoststring, int port)
	{
		//TODO: change to network/host EQUIVALENT
		_port = port;//htons(port);
		_host = inet_addr(hoststring.c_str());
		_hoststring = hoststring;
		setAddress();
	}

	void	Server::setAddress()
	{
		memset((char *)&_addr, 0, sizeof(_addr)); 
    	_addr.sin_family = AF_INET;
    	_addr.sin_port =  htons(_port);
    	_addr.sin_addr.s_addr = _host;
		_addrlen = sizeof(_addr);
	}

	int		Server::getsocketfd()
	{
		return _fd;
	}



	void		Server::setIndex(int i)
	{
		_index = i;
	}
	int			Server::getIndex()
	{
		return _index;
	}


	void		Server::setName(std::string name)
	{
		_name = name;
	}
	std::string	Server::getName()
	{
		return _name;
	}


		int			Server::getPort()
		{
			return (_port);
		}
		std::string	Server::getHost()
		{
			return(_hoststring);
		}

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
{

}

// Server::Server( const Server & src )
// {
	
// }

Server::Server(unsigned int host, int port)
{
	_host = host;
	_port = port;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

// Server &				Server::operator=( Server const & rhs )
// {
// 	//if ( this != &rhs )
// 	//{
// 		//this->_value = rhs.getValue();
// 	//}
// 	return *this;
// }

// std::ostream &			operator<<( std::ostream & o, Server const & i )
// {
// 	//o << "Value = " << i.getValue();
// 	return o;
// }







/* ************************************************************************** */



int		Server::CGI_D_ayoub(_body * bd, std::string	request_target , std::string	my_method)
{

	size_t			pos;
	std::string extention;

	extention = "";
	
	pos = request_target.find(".php");
	if (pos != std::string::npos)
		extention = request_target.substr(pos);
	else
		return 0;

	int cgi = 0;
	int query;
	query = extention.find('?');


	std::string querry;
	std::string cgi_location =  bd->_ok.get_server(_index).get_cgi().get_cgi_path();
	std::string executable_script = request_target.substr(1);


	int f_fd;
	int poss;
	if (query != -1)
	{
		querry = extention.substr(query + 1);
		extention = extention.substr(0,query);

		poss = request_target.find('?');
		if (poss != -1)
			executable_script = request_target.substr(1,poss - 1);
		else
			executable_script = request_target;
		//check if the exec-file exist
		
		if ((f_fd = open(executable_script.c_str(), O_RDWR)) < 0 && extention != "")
			return 0;
		else if ((f_fd = open(executable_script.c_str(), O_RDWR)) < 0)
			return -1;

			
		//close (f_fd);//close the file descripto of the check
	}
	else if ((f_fd = open((request_target.substr(1,request_target.size())).c_str(), O_RDWR)) < 0 && extention != "")
	{
		std::cout << "1 " << f_fd <<" " << request_target.substr(1,request_target.size()).c_str() << " " << extention << std::endl;
		return 0;
	}
	else if ((f_fd = open((request_target.substr(1,request_target.size())).c_str(), O_RDWR)) < 0)
	{
		std::cout << "2 " << f_fd <<" " << request_target.substr(1,request_target.size()).c_str() << " "<< std::endl;
		return -1;
	}	
		
	close (f_fd);//close the file descripto of the check
	//CGI IMPLEMENTATION
	if (extention == ".php")
	{
		cgi = 1;
		int fd = open("/tmp/test", O_RDWR | O_CREAT, 0777);
		int body = open("/tmp/body", O_RDWR | O_CREAT, 0777);
		if (my_method == "POST")
		{
			std::string bodyy = bd->_body_stream.str();
			write(body,bodyy.c_str(),bodyy.size());
			close(body);
		}
		int fork_id = fork();

		if (fork_id == -1)
		{
			std::cout << "error on fork forking" << std::endl;

			remove("/tmp/test");
			remove("/tmp/body");
			close(body);
			close(fd);

			return (-2);//internal error 500
		}
		else if (fork_id == 0)
		{
			
			dup2(fd,1);
			close(fd);
			if (my_method == "POST")
			{
				body = open("/tmp/body", O_RDWR , 0777);
				dup2(body, 0);
				close(body);
			}
			else
				dup2(-1, 0);
			char *args[3];
			args[0] = (char *)cgi_location.c_str();
			args[1] = (char *)executable_script.c_str();
			args[2] = NULL;

			std::vector<std::string> env;

			env.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
			env.push_back(std::string("SERVER_SOFTWARE=webserv"));
			env.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
			env.push_back(std::string("SERVER_PORT=") + std::to_string( bd->_http.get_my_port() ));
			env.push_back(std::string("REQUEST_METHOD=") + my_method);
			env.push_back(std::string("PATH_INFO=") + executable_script);
			env.push_back(std::string("QUERY_STRING=") + querry);
			env.push_back(std::string("CONTENT_TYPE=") + bd->_http.get_value("Content-Type"));
			env.push_back(std::string("CONTENT_LENGTH=") + bd->_http.get_value("Content-Length"));
			env.push_back(std::string("HTTP_USER_AGENT=") + bd->_http.get_value("User-Agent"));
			env.push_back(std::string("REDIRECT_STATUS=true"));
			env.push_back(std::string("SCRIPT_FILENAME=") + executable_script);
			char **env_arr = new char*[env.size() + 1];

			for (size_t i = 0; i < env.size(); ++i)
				env_arr[i] = strdup(env[i].c_str());
			env_arr[env.size()] = NULL;

			if (execve(cgi_location.c_str(),args,env_arr) == -1)
			{
				perror("Could not execute");

			}
			
		}
		else //parent
		{
			int timout = 0;
			int status;
			time_t start = time(NULL);
			while(difftime(time(NULL),start) <= 5)
			{
				int ret = waitpid(fork_id, &status, WNOHANG);
				if (ret == 0)
					timout = 1;
				else
				{
					timout = 0;
					break;
				}
			}
			if (timout)
			{
				std::cout << "erreur timeout" << std::endl;
				kill(9,fork_id);
				{
					remove("/tmp/test");
					remove("/tmp/body");
					close(body);
					close(fd);
					return -3;
				}
			}
			int nbytes;
			char cgi_buff[1024] = {0};

			lseek(fd, 0, SEEK_SET);
			std::string ret;
			while ((nbytes = read(fd, cgi_buff, 1024)) > 0)
			{
				ret.append(cgi_buff, nbytes);
			}
			
			bd->response = bd->_ok.parse_response_cgi(ret) ;
			bd->_ok.set_hello(bd->response);
			// std::cout << response << std::endl;
			remove("/tmp/test");
			remove("/tmp/body");
			close(body);
			close(fd);
		}
	}
	return cgi;
}