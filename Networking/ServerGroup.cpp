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

VirtualServer		*ServerGroup::addvirtualserver(std::string host, int port)
{
	std::map<listener *, VirtualServer *>::iterator beg = _vrtl_server_map.begin();
	std::map<listener *, VirtualServer *>::iterator end = _vrtl_server_map.end();
	while(beg != end)
	{
		if (beg->first->_host == host && beg->first->_port == port)
		{
			std::cout << "found virtual server HOST : " << host << 
			" PORT : " << port << std::endl;
			return (beg->second);
		}
		beg++;
	}
	return (NULL);
}

void					ServerGroup::build()
{
	FD_ZERO(&_masterfds);
	FD_ZERO(&_masterwritefds);
	_fd_size = my_confs.size();
	_servercount = _fd_size;
	_fd_cap = 0;

	for (int i = 0; i < _servercount; i++)
	{
		Server		*currentsrv = new Server();
		int			fd;


		std::string tmp_host = my_confs[i].get_host();
		int tmp_port = my_confs[i].get_Port();
		std::string tmp_name = my_confs[i].get_server_name();

////virtual servers
		VirtualServer *tmpvrtsrvr = addvirtualserver(tmp_host, tmp_port);
		listener *lstn;
		if ( tmpvrtsrvr == NULL)
		{
			lstn = new listener();
			lstn->_host = tmp_host;
			lstn->_port = tmp_port;
			lstn->_name = tmp_name;
			//std::cout << tmp_name << std::endl;

			// VirtualServer *vrtsrvr = new VirtualServer();
			tmpvrtsrvr =new VirtualServer();
			// vrtsrvr->_virtual_server_list.push_back(currentsrv);
			tmpvrtsrvr->_virtual_server_list.push_back(currentsrv);
			// _servername_map.insert(std::make_pair(tmp_name, vrtsrvr));
			_servername_map.insert(std::make_pair(tmp_name, tmpvrtsrvr));
			// _vrtl_server_map.insert(std::make_pair(lstn, vrtsrvr));
			_vrtl_server_map.insert(std::make_pair(lstn, tmpvrtsrvr));
		}
		else
		{
			lstn = new listener();
			lstn->_host = tmp_host;
			lstn->_port = tmp_port;
			lstn->_name = tmp_name;
			_servername_map.insert(std::make_pair(tmp_name, tmpvrtsrvr));
			tmpvrtsrvr->_virtual_server_list.push_back(currentsrv);
			//std::cout << "saame host/port servers coutn : " << tmpvrtsrvr->_virtual_server_list.size() << std::endl;
		}

		std::cout << tmpvrtsrvr->_virtual_server_list.size() << std::endl;
///////////


		currentsrv->setSocket(tmp_host, tmp_port);
		currentsrv->setName(tmp_name);
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


	// for (int i = 0; i < _servercount; i++)
	// {
	// 	Server		*currentsrv = new Server();
	// 	int			fd;

	// 	std::string host = my_confs[i].get_host();
	// 	int port = my_confs[i].get_Port();
	// 	std::string name = my_confs[i].get_server_name();
	// 	std::cout << "server name : " << name << std::endl;
	// 	currentsrv->setSocket(host, port);
	// 	currentsrv->setName(name);
	// 	if (currentsrv->Create() != -1)
	// 	{
	// 		fd = currentsrv->getsocketfd();
	// 		FD_SET(fd, &_masterfds);
	// 		_servers_map.insert(std::make_pair(fd, currentsrv));
	// 		currentsrv->setIndex(i);	

	// 		if (fd > _fd_cap)
	// 			_fd_cap = fd;
	// 	}
	// }
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
		//resetFDCap();

		if (select((int)_fd_cap + 1, &_readset, &_writeset, NULL, &timetostop) < 0)
			throw SelectException();
		for (size_t i = 0; i <= _fd_cap ; i++)
		{
			if (FD_ISSET(i, &_writeset) || FD_ISSET(i, &_readset))
			{
				if (isServerFD(i)) //check if the server fd is the one wich is ready if true accept client connection
				{
					int new_socket = acceptCon(i);
					if (new_socket > 0)
					{
						std::cout << "connection is accepted : " << new_socket << std::endl;
						FD_SET(new_socket, &_masterfds);
						if (new_socket > _fd_cap)
							_fd_cap = new_socket;
					}
					
					// //accept connection
					// std::map<int, Server *>::iterator it;
					// it = _servers_map.find(i);
					// if (it ==  _servers_map.end())
					// {
					// 	std::cout << "ERROR IN SERVERS MAP accept\n";
					// 	//exit(EXIT_FAILURE);
					// }
					// std::cout << "fd : " << it->second->getsocketfd() << " index : " << it->second->getIndex() << std::endl;
					// int new_socket = (it)->second->accept();
					// if (new_socket > 0)
					// {
					// 	std::cout << "connection is accepted : " << new_socket << std::endl;
					// 	FD_SET(new_socket, &_masterfds);
					// 	if (new_socket > _fd_cap)
					// 		_fd_cap = new_socket;

					// 	_client_fds.insert(std::make_pair(new_socket, it->second));
					// }
				}
				else
				{

					if (FD_ISSET(i, &_readset)) //coonection is to be read from
					{
						int flag ;
						flag = recvCon(i);

						std::map<int , _body *>::iterator it;
						it = _requests_map.find(i);


						if (flag == -1)
						{
							FD_CLR(i, & _masterfds);
							if (it != _requests_map.end())
								_requests_map.erase(it); //_requests_map; equivalent 
							close(i);
						}
						else if (flag >= 0)
						{
							FD_CLR(i, & _masterfds);
							FD_SET(i, & _masterwritefds);
						}

						// int flag;
						// std::map<int, Server *>::iterator it;
						// it = _client_fds.find(i);
						// if (it == _client_fds.end())
						// {
						// 	std::cout << "ERROR IN SERVERS Client MAP\n";
						// 	exit(EXIT_FAILURE);
						// }

						// flag = (it)->second->recv(i);
						// if (flag == -1)
						// {
						// 	FD_CLR(i, & _masterfds);
						// 	_client_fds.erase(it);
						// 	close(i);
						// }
						// else if (flag >= 0)
						// {
						// 	FD_CLR(i, & _masterfds);
						// 	FD_SET(i, & _masterwritefds);
						// }
											// else if (flag > 0)
											// {
											// 	FD_CLR(i, & _masterfds);
											// 	FD_SET(i, & _masterwritefds);
											// }
					}
					else if (FD_ISSET(i, &_writeset)) // connection is ready to be written to
					{
						int flag;
						std::map<int, _body *>::iterator it;
						Server 	*servr;
						_body	*bd;
						it = _requests_map.find(i);
						if (it == _requests_map.end())
						{
							std::cout << "ERROR IN SERVERS MAP response\n";
							FD_CLR(i, & _masterwritefds);
       						close(i);
						}
						servr = (it)->second->srvr;

						bd = (it)->second;
						flag = servr->send(i, bd);

						if (flag == 0)
						{
							_requests_map.erase(it);
							FD_CLR(i, & _masterwritefds);
       						close(i);
						}


						// std::map<int, Server *>::iterator it;
						// it = _client_fds.find(i);
						// if (it == _client_fds.end())
						// {
						// 	std::cout << "ERROR IN SERVERS MAP response\n";
						// 	FD_CLR(i, & _masterwritefds);
       					// 	close(i);
						// 	//exit(EXIT_FAILURE);
						// }
						// int flag;
						// flag = (it)->second->send(i);

						// if (flag == 0)
						// {
						// 	_client_fds.erase(it);
						// 	FD_CLR(i, & _masterwritefds);
       					// 	close(i);
						// }
					}
				}
			}
			else
			{

			}
		}
	}
}

void					ServerGroup::stop()
{
	
}
bool ServerGroup::is_number(std::string s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

Server	*ServerGroup::getHostServer(std::string servername, std::string host , int port)
{
	std::map<std::string , VirtualServer *>::iterator it;
	it = _servername_map.begin();
	while (it != _servername_map.end())
	{
		std::vector<Server *>::iterator it2;
		it2 = it->second->_virtual_server_list.begin();
		VirtualServer *tmp_vrtsrvr;
		tmp_vrtsrvr = it->second;
		while (it2 != tmp_vrtsrvr->_virtual_server_list.end())
		{
			if (tmp_vrtsrvr->_virtual_server_list.size() > 1)
			{
				if ((*it2)->getName() == servername)
					return (*it2);

			}
			else
			{
				if ((*it2)->getHost() == host && (*it2)->getPort() == port)
					return (*it2);
			}
			it2++;
		}
		it++;
	}
	return NULL;

	/*
	// std::map<std::string , VirtualServer *>::iterator it;
	// std::vector<Server *>::iterator it3;

	// // if (servername != host)
	// if (!is_number(servername))
	// {
	// 	std::cout << "hello" <<std::endl;
	// 	it = _servername_map.find(servername);
	// 	if (it == _servername_map.end())
	// 		std::cout << "flewla " << std::endl;

	// 	it3 = it->second->_virtual_server_list.begin();

	// 	if (it3 == it->second->_virtual_server_list.end())
	// 		std::cout << "ftalta " << std::endl;

	// 	while (it3 != it->second->_virtual_server_list.end())
	// 	{

	// 		if (servername == (*it3)->getName())
	// 		{
	// 			std::cout << "request host : " << host << std::endl;
	// 			std::cout << "request port : " << port << std::endl;
	// 			std::cout << "request name : " << servername << std::endl;
	// 			return ((*it3));
	// 		}
	// 		it3++;
	// 	}
	// 	return (NULL);
	// }
	// else 
	// {
	// 	std::map<int, Server *>::iterator it4;
	// 	it4 = _servers_map.begin();
	// 	while (it4 != _servers_map.end())
	// 	{
	// 		////////////////
	// 		if (it4->second->getHost() == host && it4->second->getPort() == port)
	// 		{
	// 			std::cout << "request host : " << host << std::endl;
	// 			std::cout << "request port : " << port << std::endl;
	// 			std::cout << "request name : " << servername << std::endl;
	// 			return (it4->second);
	// 		}
	// 		it++;
	// 	}
	// }
	// return (NULL);

	*/
	
}

int		ServerGroup::recvCon(int fd)
{
	int flag;
	_body *bd;

	std::map<int , _body *>::iterator it;
	it = _requests_map.find(fd);

	if (it == _requests_map.end())
	{
		Server *tmp_srvr;
		std::string tmp_servername;
		std::string tmp_host;
		int			tmp_port;

		bd = new _body();
		flag = bd->_http.handle_http_request(fd, bd->_body_file, bd->_body_size, bd->_body_stream);
		tmp_servername = bd->_http.get_my_host();
		tmp_host = bd->_http.get_my_host();
		tmp_port = bd->_http.get_my_port();


		tmp_srvr = getHostServer(tmp_servername, tmp_host ,tmp_port);
		if (tmp_srvr != NULL)
		{
			bd->srvr = tmp_srvr;
			_requests_map.insert(std::make_pair(fd, bd));
		}
		else
		{
			return -1;
		}
	}
	else
	{
		bd = it->second;
		flag = bd->_http.handle_http_request(fd, bd->_body_file, bd->_body_size, bd->_body_stream);
	}	
	return (flag);
}


int ServerGroup::acceptCon(int fd)
{
	int clnt;
	struct sockaddr_in			_addr;
	int 						_addrlen;
	if ((clnt = ::accept(fd, (struct sockaddr *)&_addr, (socklen_t*)&_addrlen)) < 0)
    {
        return (-1);
    }
	else
	{
		fcntl(clnt, F_SETFL, O_NONBLOCK);
		return (clnt);
	}
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