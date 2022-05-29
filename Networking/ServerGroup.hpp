#ifndef SERVERGROUP_HPP
# define SERVERGROUP_HPP

#include "utilities_.hpp"
#include "Server.hpp"
#include "DEBUGWS.hpp"
#include "../HTTP/HttpRequest.hpp"
#include "../HTTP/Response.hpp"
#include "../HTTP/Servers.hpp"

	class listener{
		public :
		listener(){};
		~listener(){};
		std::string _host;
		int			_port;
		std::string _name;
	};
	class	VirtualServer{
		public :
		VirtualServer(){};
		~VirtualServer(){};
		std::vector<Server *> _virtual_server_list;
	};





class ServerGroup
{

	public:

		ServerGroup(char *configfile);
		ServerGroup( ServerGroup const & src );
		~ServerGroup();

		ServerGroup &		operator=( ServerGroup const & rhs );


		void	build();
		void	start();
		void	stop();

		Server			*getHostServer(std::string servername, std::string host, int port);
		VirtualServer	*addvirtualserver(std::string host, int port);




		int		acceptCon(int fd);
		int		sendCon();
		int		recvCon(int fd);


		bool	isServerFD(int fd);
		void	resetFDCap();
		bool 	is_number(std::string s);
	private:





	std::map<int, Server *>			_client_fds;
	std::map<int, Server *>			_servers_map;

	std::map<std::string , VirtualServer *>			_servername_map;
	// std::map<std::string ,listener *>			_servername_map;
	std::map<listener *, VirtualServer *>		_vrtl_server_map;


	std::map<int , _body *>						_requests_map;


/////////////////////////////////////////////
	int								_servercount;


	fd_set							_masterfds;
	fd_set							_masterwritefds;
	unsigned int					_fd_size;
	unsigned int					_fd_cap;

	fd_set 							_readset;
    fd_set 							_writeset;
	fd_set							_selected;






	Servers					_ok;
	std::vector<Conf>       my_confs;




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