// #ifndef SOCKET_HPP
// # define SOCKET_HPP

# include <iostream>
# include <string>
#include <sys/socket.h>
#include <netinet/in.h>

namespace webserv
{
	class Socket
	{

		public:
			Socket();
			Socket(int domain, int service, int protocol);
			Socket( Socket const & src );
			~Socket();

			Socket &		operator=( Socket const & rhs );

			int				getConnection() const ;
			void			setConnection(int domain, int service, int protocol);

		private:
			int 	connection;

	};

	std::ostream &			operator<<( std::ostream & o, Socket const & i );

}
// #endif /* ********************************************************** SOCKET_H */