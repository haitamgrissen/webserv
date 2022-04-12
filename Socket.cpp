#include "Socket.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

webserv::Socket::Socket(int domain, int service, int protocol)
{
	connection = socket(domain, service, protocol);
	if (connection == 0)
		std::cout << "matcreash lrbk" << std::endl;
	else if (connection > 0)
		std::cout << "tcrea lrbk" << std::endl;

}

webserv::Socket::Socket( const Socket & src )
{
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
	return *this;
}

std::ostream &			webserv::operator<<( std::ostream & o, Socket const & i )
{
	//o << "Value = " << i.getValue();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */