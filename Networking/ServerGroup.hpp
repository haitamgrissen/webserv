#ifndef SERVERGROUP_HPP
# define SERVERGROUP_HPP

#include "utilities_.hpp"
#include "Server.hpp"
#include "DEBUGWS.hpp"
#include "../HTTP/HttpRequest.hpp"
#include "../HTTP/Response.hpp"

class ServerGroup
{

	public:

		ServerGroup();
		ServerGroup( ServerGroup const & src );
		~ServerGroup();

		ServerGroup &		operator=( ServerGroup const & rhs );


		void	build();
		void	start();
		void	stop();

		int		acceptCon(int fd);
		int		sendCon();
		int		recvCon();


		bool	isServerFD(int fd);

	private:

	std::vector<std::string>	_hostslist;
	std::vector<int>			_portslist;
	std::map<int, Server>		_servers_map;
	std::vector<Server>				_servers_vec;


	std::vector<int>			_client_fds;

/////////////////////////////////////////////
	int								_servercount;


	fd_set							_masterfds;
	fd_set							_masterwritefds;
	unsigned int					_fd_size;
	unsigned int					_fd_cap;

	fd_set 							_readset;
    fd_set 							_writeset;
	fd_set							_selected;









	//exceptions
	
	struct BuildException : public std::exception 
	{
   		const char * what () const throw () 
		{
      		return "Server Couldn't Build ServerGroup";
   		}
	};

	struct SelectException : public std::exception 
	{
   		const char * what () const throw () 
		{
      		return "Server Couldn't Select Connection";
   		}
	};

	struct AcceptException : public std::exception 
	{
   		const char * what () const throw () 
		{
      		return "Server Couldn't Accept connection";
   		}
	};


	DebugWS 						Debugging;
};

std::ostream &			operator<<( std::ostream & o, ServerGroup const & i );

#endif /* ***************************************************** SERVERGROUP_H */