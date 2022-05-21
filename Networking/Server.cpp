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
	//int clnt = accept(_fd, (struct sockaddr * ) &_addr, sizeof(_addr));
	int clnt;
	if ((clnt = ::accept(_fd, (struct sockaddr *)&_addr, (socklen_t*)&_addrlen)) < 0)
    {
		std::cout << "[   ]" <<  _fd << "[   ]"<< "hello from accept" << std::endl;
        perror("In accept");
        exit(EXIT_FAILURE);
    }
	fcntl(clnt, F_SETFL, O_NONBLOCK);

	_requestmap.insert(std::make_pair(clnt, new _body()));
	return (clnt);
}

int		Server::send(int sock)
{
	std::map<int, _body *>::iterator	it;
	it = _requestmap.find(sock);
	_body *bd = it->second;

	if (bd->_http.Get_Http_Method() == "POST")
	{
		bd->_body_file << bd->_body_stream.str() << std::endl;
		bd->_body_file.close();        
	}

	if (bd->_http.Get_Http_Method() == "POST" && bd->_http.get_value("Transfer-Encoding") == "chunked")
	{
		std::cout << "ok?" << std::endl;
		bd->_http.handle_chunked_body();
	}
	else if (bd->_http.Get_Http_Method() == "POST")
		bd->_http.handle_regular_body();


	Response ok;
	ok.setIndex(_index);
	ok.set_mybuffer(bd->_http.Get_Request_Target());
	if (bd->_http.Get_Http_Method() == "GET")
	    bd->_body_size = ok.handle_Get_response();
	else if (bd->_http.Get_Http_Method() == "DELETE")
	    ok.handle_delete_response(bd->_http.get_value("Connection"));
	else if (bd->_http.Get_Http_Method() == "POST")
	    ok.handle_post_response(bd->_http.get_value("Connection"));
	
	write(sock , ok.get_hello() , ok.get_total_size());



	_requestmap.erase(it);
	return 0;
}
int		Server::recv(int sock)
{
	std::map<int, _body *>::iterator	it;


	it = _requestmap.find(sock);
	if (it ==  _requestmap.end())
	{
		std::cout << "couldnt receve request\n";
		//exit(EXIT_FAILURE);
	}
	_body *bd = it->second;
	bd->_http.handle_http_request(sock, bd->_body_file, bd->_body_size, bd->_body_stream);
	//it->second->handle_http_request(sock, _body_file, _body_size, _body_stream);
	//http.handle_http_request(sock, _body_file, _body_size, _body_stream);
	return 0;
}

/*
** --------------------------------- Modefiers ---------------------------------
*/

int Server::SAcceptCon()
{
	// std::cout << "hello from accept§" << std::endl;
	// //int clnt = accept(_fd, (struct sockaddr * ) &_addr, sizeof(_addr));
	// int clnt;
	// if ((clnt = ::accept(_fd, (struct sockaddr *)&_addr, (socklen_t*)&_addrlen)) < 0)
    // {
    //     perror("In accept");
    //     exit(EXIT_FAILURE);
    // }
	// fcntl(clnt, F_SETFL, O_NONBLOCK);

	// return (clnt);
	return 0;
}

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
    	_addr.sin_port =  htons(_port);//htons(_port);
		//std::cout << _host << std::endl;
    	_addr.sin_addr.s_addr = _host;//INADDR_ANY;//_host;//htonl(_host);0.0.0.0 127.0.0.1
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