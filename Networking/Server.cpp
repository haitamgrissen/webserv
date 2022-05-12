#include "Server.hpp"



/*
** --------------------------------- METHODS ----------------------------------
*/



int Server::Create()
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
	{
		std::cout << "Error creating server Socket" << std::endl;
		exit(0);
	}
	

	if (bind(_fd, (struct sockaddr * ) &_addr, sizeof(_addr)) == -1)
	{
		std::cout << "Error IN Bind " << std::endl;
		exit(0);
	}
	if (listen(_fd, 10000) < 0)
	{
		std::cout << "Error IN listen" << std::endl;
		exit(0);
	}


	//std::cout << "server fd : " << _fd << std::endl;
	std::cout <<  "host : " << _hoststring << " IP : " << _host << " port : " << _port << " is up"  << std::endl;
	std::cout << "++++++++" << std::endl;
	//std::cout << "server host : " << _hoststring << std::endl;
	return 0;
}

/*
** --------------------------------- Modefiers ---------------------------------
*/

int Server::SAcceptCon()
{
	std::cout << "hello from accept§" << std::endl;
	//int clnt = accept(_fd, (struct sockaddr * ) &_addr, sizeof(_addr));
	int clnt;
	if ((clnt = accept(_fd, (struct sockaddr *)&_addr, (socklen_t*)&_addrlen)) < 0)
    {
        perror("In accept");
        exit(EXIT_FAILURE);
    }
	fcntl(clnt, F_SETFL, O_NONBLOCK);

	return (clnt);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

	void 	Server::setSocket(std::string hoststring, int port)
	{
		//TODO: change to network/host EQUIVALENT
		_port = htons(port);
		_host = inet_addr(hoststring.c_str());
		_hoststring = hoststring;
		setAddress();
	}

	void	Server::setAddress()
	{
		memset((char *)&_addr, 0, sizeof(_addr)); 
    	_addr.sin_family = AF_INET;
    	_addr.sin_port =  htons(8880);//htons(_port);
    	_addr.sin_addr.s_addr = INADDR_ANY;//_host;//htonl(_host);
		_addrlen = sizeof(_addr);
	}

	int		Server::getsocketfd()
	{
		return _fd;
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