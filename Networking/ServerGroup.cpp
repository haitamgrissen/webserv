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
	FD_ZERO(&_masterwritefds);
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
	


	std::fstream    body_file;
	size_t          body_size;
	HttpRequest http;
	std::ostringstream  body_stream;

	body_size = 0;
	body_file.open("body.txt", std::ios::out);

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
					int new_socket = acceptCon(i); 
					std::cout << "connection is accepted :" << new_socket << std::endl;
					FD_SET(new_socket, &_masterfds);
					if (new_socket > _fd_cap)
						_fd_cap = new_socket;
				}
				else
				{
					if (FD_ISSET(i, &_readset)) //coonection is to be read from
					{
						int flag;
							
						flag = http.handle_http_request(i, body_file, body_size, body_stream);
						if (!flag)
						{
							FD_CLR(i, & _masterfds);
							FD_SET(i, & _masterwritefds);
						}
						//exit(0);


						// int ret ; 
						// //read from connected socket
						// char buffer[30000];
						// 	// printf("[%s]÷\n", buffer);
        				// ret = read( i , buffer, sizeof(buffer)); //3000
						// /*
						// 	if (is first)
						// 		request (buffer)
						// 	else 
						// 		request.append(buffer);

						// 		if (request.ifFinshed())
						// */
						// std::cout << "Read :" << ret << " " << i  << std::endl;
						// if (ret == 0 || ret == -1)
						// {
						// 	if (ret  == 0)
						// 	{
						// 		std::cout << "Connection Closed form client" << std::endl;
						// 		close(i);
						// 	FD_CLR(i , &_masterfds);

						// 	}
						// 	else 
						// 		perror("in read");
						// 	//exit(1);
						// }
						// else
						// {
						// 	FD_CLR(i, & _masterfds);
						// 	FD_SET(i, & _masterwritefds);
						// }


					}
					else if (FD_ISSET(i, &_writeset)) // connection is ready to be written to
					{
						if (http.Get_Http_Method() == "POST")
						{
							body_file << body_stream.str() << std::endl;
							body_file.close();        
						}

						if (http.Get_Http_Method() == "POST" && http.get_value("Transfer-Encoding") == "chunked")
						{
							std::cout << "ok?" << std::endl;
							http.handle_chunked_body();
						}
						Response ok;
       					ok.set_mybuffer(http.Get_Request_Target());
       					if (http.Get_Http_Method() == "GET")
       					    body_size = ok.handle_Get_response();
       					else if (http.Get_Http_Method() == "DELETE")
       					    ok.handle_delete_response(http.get_value("Connection"));
       					else if (http.Get_Http_Method() == "POST")
       					    ok.handle_post_response(http.get_value("Connection"));
						
       					write(i , ok.get_hello() , ok.get_total_size());
						FD_CLR(i, & _masterwritefds);
       					close(i);
						// //write to connected socket
						// std::cout << "SENT RESPONSE\n";
						// char *hello = strdup("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 16\n\nNizaaaaaaaaar!!!");
        				// write(i , hello , strlen(hello));
						// //buufet 
						// FD_CLR(i, & _masterwritefds);

						// close(i);
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