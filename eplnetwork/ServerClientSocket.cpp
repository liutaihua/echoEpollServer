#include <stdexcept>
#include <iostream>
#include <fstream>
#include "ServerClientSocket.h"
#include "Py/ScriptManager.h"


SClientSocket::SClientSocket(SOCKET fd,const sockaddr_in * peer) :
  TcpSocket(fd,peer)
{
}


SClientSocket::~SClientSocket()
{
    std::cout<<"test```"<<"\n";
}

void SClientSocket::OnRecvData()
{
//    STNetMsgHeader temp;
    char temp[64];
    while (true)
    {
        if (GetReadBuffer()->GetSize() < sizeof(temp)){
            std::cout<<"not enough recv"<<GetReadBuffer()->GetSize()<<"\n";
            return;
        }
        GetReadBuffer()->Read((uint8*)&temp, sizeof(temp));
        break;
    }
    std::string str = temp;
    std::cout<<"on recv data: "<<str<<"\n";
    Write(&temp, sizeof(temp));
    // 测试在PY里调用SClientSocket->Disconnect() 去掉注释将会直接被断开这个连接
    //ScriptManager::Instance()->CallNoRT("eplObserver.print_recv_data", dynamic_cast<TcpSocket*>(this));

}

void SClientSocket::OnDisconnect()
{
    std::cout<<"test```"<<"\n";
}

void SClientSocket::OnConnect()
{
    std::cout<<"test```"<<"\n";
}





