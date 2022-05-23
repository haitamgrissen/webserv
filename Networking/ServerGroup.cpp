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
			//_servers_vec.push_back(currentsrv);

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

	while (true)
	{
		_readset = _masterfds;
		_writeset = _masterwritefds;
		if (select((int)_fd_cap + 1, &_readset, &_writeset, NULL, NULL) < 0)
			throw SelectException();

		for (size_t i = 0; i < _fd_cap + 1; i++)
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
						exit(EXIT_FAILURE);
					}
						//current server recv
					
					std::cout << "fd :" << it->second->getsocketfd() << "index :" << it->second->getIndex() << std::endl;
					int new_socket = (it)->second->accept();
					//int new_socket = acceptCon(i); 
					std::cout << "connection is accepted : " << new_socket << std::endl;
					FD_SET(new_socket, &_masterfds);
					if (new_socket > _fd_cap)
						_fd_cap = new_socket;

					_client_fds.insert(std::make_pair(new_socket, it->second));
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
						if (!flag)
						{
							FD_CLR(i, & _masterfds);
							FD_SET(i, & _masterwritefds);
						}
					}
					else if (FD_ISSET(i, &_writeset)) // connection is ready to be written to
					{

						std::map<int, Server *>::iterator it;
						it = _client_fds.find(i);
						if (it == _client_fds.end())
						{
							std::cout << "ERROR IN SERVERS MAP response\n";
							exit(EXIT_FAILURE);
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
		}
	}
}

void					ServerGroup::stop()
{
	
}

/*
** --------------------------------- HANDELINGS ---------------------------------
*/
int		ServerGroup::acceptCon(int fd)
{
	int newsocket;
	struct sockaddr_in newaddr;
	unsigned int addrlen;
	
	newsocket = accept(fd , (struct sockaddr *)&newaddr, (socklen_t*)&addrlen);
	fcntl(newsocket, F_SETFL, O_NONBLOCK);

	if (newsocket < 0)
		throw AcceptException();
	//_client_fds.push_back(newsocket);
	return newsocket;
}

int		ServerGroup::sendCon()
{
	return 0;
}
int		ServerGroup::recvCon()
{
	return 0;
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

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */