#include "./Networking/utilities_.hpp"
#include "./Networking/ServerGroup.hpp"


int main()
{
    ServerGroup serverG;

  try 
  {

    serverG.build();
    serverG.start();
  } 
  catch (const std::exception& e) 
  {
    std::cerr << e.what() << std::endl;
  }
    //parse config file

    // build server groupes

    // start server groupe

    // stop server groupe

    return (0);
}