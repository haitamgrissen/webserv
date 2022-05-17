#ifndef SERVER_HPP
# define SERVER_HPP

#include "utilities_.hpp"

class Server
{

	public:

		int Create();

		int SAcceptCon();


		//NETWORK I/O
		int		accept();
		int		send();
		int		recv();

		//ACCESSORS
		void 	setSocket(std::string host, int port);
		int		getsocketfd();
		void	setAddress();
		


		//CONSTRUCTORS AND OVERLOADS
		Server();
		Server(unsigned int host, int port);
		Server( Server const & src );
		~Server();
		Server &		operator=( Server const & rhs );

	private:
		//std::vector<> of requests
		//std::vector<> of responses

		int							_fd;
		//socket elements
		int							_port;
		unsigned int 				_host;
		std::string 				_hoststring;



		struct sockaddr_in			_addr;
		int _addrlen;





	struct SocketException : public std::exception 
	{
   		const char * what () const throw () 
		{
      		return "Error creating server Socket";
   		}
	};
	struct BindException : public std::exception 
	{
   		const char * what () const throw () 
		{
      		return "Error IN Bind";
   		}
	};
	struct ListenException : public std::exception 
	{
   		const char * what () const throw () 
		{
      		return "Error IN listen";
   		}
	};
    
};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */