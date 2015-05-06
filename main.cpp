#include <iostream>
#include "eplnetwork/ServerClientSocket.h"

int main()
{
    CreateSocketEngine();
    sSocketEngine.SpawnThreads();
    if  (CreateListenSocket<SClientSocket>("0.0.0.0", 8886))
    {
      std::cout << "server run okok "<< std::endl;
    }
    else
    {
      std::cout << "server no run "<< std::endl;
      exit(0);
    }
    while (true){

    }
    return 0;
}

