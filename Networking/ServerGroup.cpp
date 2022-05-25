#include "ServerGroup.hpp"


/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ServerGroup::ServerGroup(char *configfile)
{
	std::string mystring(configfile);
	_ok.parse_server(mystring);
	my_confs = _ok.get_server();


}

ServerGroup::ServerGroup( const ServerGroup & src )
{

}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ServerGroup::~ServerGroup()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ServerGroup &				ServerGroup::operator=( ServerGroup const & rhs )
{
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, ServerGroup const & i )
{
	//o << "Value = " << i.getValue();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

void					ServerGroup::build()
{


	FD_ZERO(&_masterfds);
	FD_ZERO(&_masterwritefds);
	_fd_size = my_confs.size();
	_servercount = _fd_size;
	_fd_cap = 0;

	//loop over all server_sockets create and  fill the _servers map
	for (int i = 0; i < _servercount; i++)
	{
		Server		*currentsrv = new Server();
		int			fd;

		std::string host = my_confs[i].get_host();
		int port = my_confs[i].get_Port();

		currentsrv->setSocket(host, port);
		if (currentsrv->Create() != -1)
		{
			fd = currentsrv->getsocketfd();
			FD_SET(fd, &_masterfds);
			_servers_map.insert(std::make_pair(fd, currentsrv));
			currentsrv->setIndex(i);	

			if (fd > _fd_cap)
				_fd_cap = fd;
		}
	}
	if (_fd_cap == 0)
		throw BuildException();
}

void					ServerGroup::start()
{
	struct timeval	timetostop;
	timetostop.tv_sec  = 1;
	timetostop.tv_usec = 0;
	signal(SIGPIPE, SIG_IGN);//ignore sigPipe
	while (true)
	{
		_readset = _masterfds;
		_writeset = _masterwritefds;
		resetFDCap();

		if (select((int)_fd_cap + 1, &_readset, &_writeset, NULL, &timetostop) < 0)
			throw SelectException();
		for (size_t i = 0; i <= _fd_cap ; i++)
		{
			if (FD_ISSET(i, &_writeset) || FD_ISSET(i, &_readset))
			{
				if (isServerFD(i)) //check if the server fd is the one wich is ready if true accept client connection
				{
					//accept connection
					std::map<int, Server *>::iterator it;
					it = _servers_map.find(i);
					if (it ==  _servers_map.end())
					{
						std::cout << "ERROR IN SERVERS MAP accept\n";
						//exit(EXIT_FAILURE);
					}
					std::cout << "fd : " << it->second->getsocketfd() << " index : " << it->second->getIndex() << std::endl;
					int new_socket = (it)->second->accept();
					if (new_socket > 0)
					{
						std::cout << "connection is accepted : " << new_socket << std::endl;
						FD_SET(new_socket, &_masterfds);
						if (new_socket > _fd_cap)
							_fd_cap = new_socket;

						_client_fds.insert(std::make_pair(new_socket, it->second));
					}
				}
				else
				{
					if (FD_ISSET(i, &_readset)) //coonection is to be read from
					{
						int flag;
						std::map<int, Server *>::iterator it;
						it = _client_fds.find(i);
						if (it == _client_fds.end())
						{
							std::cout << "ERROR IN SERVERS Client MAP\n";
							exit(EXIT_FAILURE);
						}

						flag = (it)->second->recv(i);
						if (flag == -1)
						{
							FD_CLR(i, & _masterfds);
							_client_fds.erase(it);
							close(i);
						}
						else if (flag >= 0)
						{
							FD_CLR(i, & _masterfds);
							FD_SET(i, & _masterwritefds);
						}
						// else if (flag > 0)
						// {
						// 	FD_CLR(i, & _masterfds);
						// 	FD_SET(i, & _masterwritefds);
						// }
					}
					else if (FD_ISSET(i, &_writeset)) // connection is ready to be written to
					{
						std::map<int, Server *>::iterator it;
						it = _client_fds.find(i);
						if (it == _client_fds.end())
						{
							std::cout << "ERROR IN SERVERS MAP response\n";
							FD_CLR(i, & _masterwritefds);
       						close(i);
							//exit(EXIT_FAILURE);
						}
						int flag;
						flag = (it)->second->send(i);

						if (flag == 0)
						{
							_client_fds.erase(it);
							FD_CLR(i, & _masterwritefds);
       						close(i);
						}
					}
				}
			}
			else
			{
				// std::map<int, Server *>::iterator it;
				// it = _client_fds.find(i);
				// if (it != _client_fds.end())
				// {
				// 	_client_fds.erase(it);
				// 	close(i);
				// }
			}
		}
		// std::map<int, Server *>::iterator it = _client_fds.begin();
		// std::cout << "after for loop : [" << _client_fds.size() << std::endl << "] its FD : [" << it->first << "]"<< std::endl;

	}
}

void					ServerGroup::stop()
{
	
}


/*
** --------------------------------- CHECKS ---------------------------------
*/

bool	ServerGroup::isServerFD(int fd)
{
	std::map<int, Server *>::iterator it = _servers_map.find(fd);

	if (it == _servers_map.end())
		return false;
	else
		return true;
}


void	ServerGroup::resetFDCap()
{
	std::map<int, Server *>::iterator it1;
	it1 = _servers_map.begin();
	_fd_cap = 0;

	while (it1 != _servers_map.end())
	{
		if (_fd_cap < it1->first)
			_fd_cap = it1->first;
		it1++;
	}

	std::map<int, Server *>::iterator it;
	it = _client_fds.begin();
	while (it != _client_fds.end())
	{
		if (_fd_cap < it->first)
			_fd_cap = it->first;
		it++;
	}
	
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */