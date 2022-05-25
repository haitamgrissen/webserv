#include "Server.hpp"



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
		_requestmap.insert(std::make_pair(clnt, new _body()));
		return (clnt);
	}
}

int		Server::send(int sock)
{
	std::map<int, _body *>::iterator	it;

	it = _requestmap.find(sock);
	if (it == _requestmap.end())
	{
		std::cout << "ERROR IN Request MAP\n";
		exit(EXIT_FAILURE);
	}


	_body *bd;

	bd = it->second;
	
	

	if (bd->_http.Get_Http_Method() == "POST")
	{
		bd->_body_file << bd->_body_stream.str() << std::endl;
		bd->_body_file.close();        
	}






	if (it->second->_startedwrite == false)
	{
		it->second->_startedwrite = true;
	}



	bd->_ok.setIndex(_index);
	std::string	error_msg;
	bd->_ok.set_request_method(bd->_http.Get_Http_Method());
	bd->_ok.set_request_target(bd->_http.Get_Request_Target());


	bd->_ok.set_mybuffer(bd->_http.Get_Request_Target());

	bd->_ok.check_file();
	error_msg = bd->_ok.parsing_check();
	bd->_http.set_my_upload_path(bd->_ok.get_my_upload_path());


	if (bd->_http.Get_Http_Method() == "POST" && bd->_http.get_value("Transfer-Encoding") == "chunked")
		bd->_http.handle_chunked_body();
	else if (bd->_http.Get_Http_Method() == "POST")
		bd->_http.handle_regular_body();
	if (bd->_ok.get_max_body_size() < 0)
	{
		bd->_ok.error_handling("500 Webservice currently unavailable");
	}
	else if (bd->_http.get_total_size() > bd->_ok.get_max_body_size() && bd->_ok.get_max_body_size() != 0)
		bd->_ok.error_handling("413 Payload Too Large");
	else
	{
		if (error_msg != "")
			bd->_ok.error_handling(error_msg);
		else
		{
			if (bd->_http.Get_Http_Method() == "GET")
				bd->_body_size = bd->_ok.handle_Get_response();
			else if (bd->_http.Get_Http_Method() == "DELETE")
				bd->_ok.handle_delete_response(bd->_http.get_value("Connection"));
			else if (bd->_http.Get_Http_Method() == "POST")
				bd->_ok.handle_post_response(bd->_http.get_value("Connection"));
		}		
	}	

	int flag;
	flag = write(sock , bd->_ok.get_hello() + bd->_writecount , bd->_ok.get_total_size() - bd->_writecount);
	bd->_writecount += flag;


	if (flag == 0)
	{
		_requestmap.erase(it);
		return 0;
	}
	if (bd->_ok.get_total_size() <= bd->_writecount)
	{
		_requestmap.erase(it);
		return 0;
	}
	else
		return (1);

}
int		Server::recv(int sock)
{
	std::map<int, _body *>::iterator	it;
	int									flag;

	it = _requestmap.find(sock);
	if (it ==  _requestmap.end())
	{
		std::cout << "couldnt receve request\n";
		_requestmap.erase(it);
		return (-1);//TODO:ghir t9mira 
	}
	else
	{
		_body *bd = it->second;
		flag = bd->_http.handle_http_request(sock, bd->_body_file, bd->_body_size, bd->_body_stream);
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

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
{

}

Server::Server( const Server & src )
{
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

/* ************************************************************************** */