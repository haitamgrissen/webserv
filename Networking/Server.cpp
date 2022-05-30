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

int		Server::send(int sock, _body * bd)
{
	std::string	my_method;
	std::string	my_chunk;
	int			my_len;
	std::string	error_msg = "";
	std::string	request_target;
	std::string	red_target;

	bd->init_values(my_method, my_chunk, _index, my_len, request_target);
	
	if (bd->_startedwrite == false)
		bd->_startedwrite = true;
	red_target = request_target.substr(request_target.find_last_of("/") + 1, request_target.size());
	
	red_target = bd->_ok.get_server(_index).get_redirection_value(red_target);
	
	if (red_target != "")
	{
		bd->_ok.set_request_target(red_target);
		bd->_ok.handle_redirect_response(bd->_http.get_value("Connection"));
	}
	else if (my_method == "POST" && ((my_len < 0) || (bd->_body_stream.str() == "" && my_len != 0)))
		bd->_ok.error_handling("400 Bad Request");
	else
	{
		bd->set_values(my_method, error_msg);
		error_msg = bd->_ok.parsing_check(request_target);
		if (error_msg != "")
		 	bd->_ok.error_handling(error_msg);
		else if (!bd->handle_body(my_method, my_chunk, error_msg, my_len))
			bd->_ok.error_handling("400 Bad Request");
		else
		{
			if (bd->_ok.get_max_body_size() < 0)
				bd->_ok.error_handling("500 Webservice currently unavailable");
			else if (bd->_http.get_total_size() > bd->_ok.get_max_body_size() && bd->_ok.get_max_body_size() != 0)
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


	////////// ayoub

	int cgi = CGI_D_ayoub(bd, request_target, my_method);


	///TODO:this is return errors from CGI_D_ayoub(bd, request_target, my_method);
	if (cgi == -1)///executable scripte doesn exist 
	{	
		std::cout << "404 Not Found" << std::endl;
		//bd->_ok.error_handling("400 Bad Request");
		///error  not found exec file 404
		//return 0;
	}
	else if (cgi == -2)
	{	
		///bd->_ok.error_handling("500 Webservice currently unavailable");
		///internal SERVER ERROR 500
		return 0;
	}
	else if (cgi == -3)
	{	
		///bd->_ok.error_handling("500 Webservice currently unavailable");
		///internal SERVER ERROR 500
		return 0;
	}

	//////////

	int flag;

	if (cgi == 0 || cgi < 0)/////yalahuiiiiii
		flag = write(sock , bd->_ok.get_hello() + bd->_writecount , bd->_ok.get_total_size() - bd->_writecount);
	else
		flag = write(sock, bd->response.c_str() + bd->_writecount , bd->response.size() - bd->_writecount);




	bd->_writecount += flag;
	bd->_ok.clear();
	if (flag == -1 || flag == 0)
		return -1;
	else if (bd->_ok.get_total_size() <= bd->_writecount)
		return 0;
	else
		return (1);
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
	///
}

Server::Server( const Server & src )
{
	////
}

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

Server &				Server::operator=( Server const & rhs )
{
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Server const & i )
{
	//o << "Value = " << i.getValue();
	return o;
}



int		Server::CGI_D_ayoub(_body * bd, std::string	request_target , std::string	my_method)
{

	std::string extention;
	extention = request_target.substr(request_target.find("."));
	

	int cgi = 0;
	int query;
	query = extention.find('?');



	std::string querry;
	std::string cgi_location =  bd->_ok.get_server(_index).get_cgi().get_cgi_path();
	std::string executable_script = request_target.substr(1);



	if (query != -1)
	{
		querry = extention.substr(query + 1);
		extention = extention.substr(0,query);

		executable_script = request_target.substr(1,request_target.find('?') - 1);


		//check if the exec-file exist
		int f_fd;
		if ((f_fd = open(executable_script.c_str(), O_RDWR)) == -1)
			return -1;
		close (f_fd);//close the file descripto of the check
	}


	//CGI IMPLEMENTATION
	if (extention == ".py")
	{
		cgi = 1;
		int fd = open("/tmp/test", O_RDWR | O_CREAT, 0777);
		int body = open("/tmp/body", O_RDWR | O_CREAT, 0777);
	
		int fork_id = fork();

		if (fork_id == -1)
		{
			std::cout << "error on fork forking" << std::endl;
			return (-2);//internal error 500
		}
		else if (fork_id == 0)
		{
			if (my_method == "POST")
			{
				std::string bodyy = bd->_body_stream.str();
				write(body,bodyy.c_str(),bodyy.size());
				dup2(body,0);
			}
			dup2(fd,1);

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

			char **env_arr = new char*[env.size() + 1];

			for (int i = 0; i < env.size(); ++i)
				env_arr[i] = strdup(env[i].c_str());
			env_arr[env.size()] = NULL;

			if (execve(cgi_location.c_str(),args,env_arr) == -1)
				perror("Could not execute");
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
/* ************************************************************************** */