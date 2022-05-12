#include "ServerGroup.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ServerGroup::ServerGroup()//TODO: add config file as parameter
{
	//TODO: get config file data and pass it to build function

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
	//TODO:  get_allserver_sockets, replace with config file parsing equivalent
	_portslist = Debugging.getServerPorts();
	_hostslist = Debugging.getServerHosts();
	//TODO:

	FD_ZERO(&_masterfds);
	_fd_size = _hostslist.size();
	_servercount = _fd_size;
	_fd_cap = 0;

	//loop over all server_sockets create and  fill the _servers map
	for (int i = 0; i < _portslist.size(); i++)
	{
		Server		currentsrv;
		int			fd;
		currentsrv.setSocket(_hostslist[i], _portslist[i]);
		if (currentsrv.Create() != -1)
		{
			fd = currentsrv.getsocketfd();
			FD_SET(fd, &_masterfds);
			_servers_map.insert(std::make_pair(fd, currentsrv));
			
			_servers_vec.push_back(currentsrv);

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

		if (select((int)_fd_cap + 1, &_readset, &_writeset, NULL, &timetostop) < 0)
			throw SelectException();

		for (size_t i = 0; i <_fd_cap + 1; i++)
		{
			
			if (FD_ISSET(i, &_writeset) || FD_ISSET(i, &_readset))
			{
				if (isServerFD(i)) //check if the server fd is the one wich is ready if true accept client connection
				{
					//accept connection
					int new_socket = acceptCon(i); 
					std::cout << "connection is accepted" << std::endl;
					if (i > _fd_cap)
						_fd_cap = i;
					char buffer[30000] = {0};
        			read( new_socket , buffer, 30000);
					char *hello = strdup("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!");
        			write(new_socket , hello , strlen(hello));
				}
				else
				{
					if (FD_ISSET(i, &_readset)) //coonection is to be read from
					{
					 	//read from connected socket
					}
					else if (FD_ISSET(i, &_writeset)) // connection is ready to be written to
					{
						//read from connected socket
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
	FD_SET(newsocket, &_readset);
	_client_fds.push_back(newsocket);
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
	std::map<int, Server>::iterator it = _servers_map.find(fd);

	if (it == _servers_map.end())
		return false;
	else
		return true;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */