#include "Socket.hpp"
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

webserv::Socket::Socket()
{

}
webserv::Socket::Socket(int domain, int service, int protocol)
{
	connection =  socket(domain, service, protocol);
	if (connection < 0)
		std::cout << "connection socket failed to be created miserably" << std::endl;
}

webserv::Socket::Socket( const Socket & src )
{
	(void) src;//
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

webserv::Socket::~Socket()
{
	
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

webserv::Socket &				webserv::Socket::operator=( Socket const & rhs )
{
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	(void) rhs;
	return *this;

}

std::ostream &			webserv::operator<<( std::ostream & o, Socket  const & i ) 
{
	o << i.getConnection();

	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int			webserv::Socket::getConnection() const
{
	return (connection);
}

void			webserv::Socket::setConnection(int domain, int service, int protocol)
{
	connection =  socket(domain, service, protocol);
	if (connection < 0)
		std::cout << "connection socket failed to be created miserably" << std::endl;
}

/* ************************************************************************** */

