#include <iostream>
#include "eplnetwork/ServerClientSocket.h"
#include "Py/ScriptManager.h"
#include <boost/program_options.hpp>

int main(int argc, char *argv[])
{
    boost::program_options::options_description config("myconfig");
    config.add_options()
            ("port,p", boost::program_options::value<int32>()->default_value(8888), "Port to listen.")
            ;
    boost::program_options::variables_map CmdVM;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, config),CmdVM);
    boost::program_options::notify(CmdVM);


    ScriptManager::Instance()->CallNoRT("test.test", "this");
    std::cout<<"bb done"<<std::endl;
    CreateSocketEngine();
    sSocketEngine.SpawnThreads();
    if  (CreateListenSocket<SClientSocket>("0.0.0.0", CmdVM["port"].as<int32>()))
    {
      std::cout << "server run okok Listen "<<CmdVM["port"].as<int32>()<<" ..."<< std::endl;
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

