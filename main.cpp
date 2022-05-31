#include "Networking/utilities_.hpp"
#include "Networking/ServerGroup.hpp"
#include "Networking/Server.hpp"

int main(int ac, char **av)
{
  if (ac != 2)
  {
    std::cout << "Invalid Number Of Arguments"<< std::endl;
    exit(EXIT_FAILURE);
  }
  ServerGroup serverG(av[1]);

  try 
  {
    serverG.build();
    serverG.start(av[1]);
  } 
  catch (const std::exception& e) 
  {
    std::cerr << e.what() << std::endl;
  }
    //parse config file

    // build server groupesa

    // start server groupe

    // stop server groupe

    return (0);
}