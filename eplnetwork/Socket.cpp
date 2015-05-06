#include "Common.h"
#include "Network.h"
#include "Socket.h"
#include <iostream>
//initialiseSingleton(SocketGarbageCollector);


TcpSocket::TcpSocket(int fd, const sockaddr_in * peer)
{
    SetFd(fd);
    memcpy(&m_peer, peer, sizeof(sockaddr));

    m_readBuffer = new StraightBuffer;
    m_writeBuffer = new StraightBuffer;

    m_readBuffer->Allocate(1024);
    m_writeBuffer->Allocate(1024);

    m_connected = true;
    m_deleted = false;
    m_writeLock = 0;
    SocketOps::Nonblocking(m_fd);
    SocketOps::DisableBuffering(m_fd);
}

TcpSocket::~TcpSocket()
{
}

void TcpSocket::Finalize()
{
    sSocketEngine.AddSocket(this);
    OnConnect();
    std::cout<<"TcpSocket::Finalize"<<"\n";
}

void TcpSocket::OnRead(size_t len)
{
    std::cout<<"new read from socket: "<<GetFd()<<"\n";
    /* Any other platform, we have to call recv() to actually get the data. */
    // 从socket fd中读取, 放入buffer中
    std::cout<<"space: "<<m_readBuffer->GetSpace()<<"\n";
    int bytes = recv(m_fd, (char*)m_readBuffer->GetBuffer(), m_readBuffer->GetSpace(), 0);

    if(bytes <= 0)
        Disconnect();
    else
    {
        // 读取成功后, 增加待读取值written 给后续应用逻辑代码读取数量做依据
        m_readBuffer->IncrementWritten(bytes);
        OnRecvData();
    }
}

void TcpSocket::OnWrite(size_t len)
{
    std::cout<<"OnWrite to fd"<<"\n";
    LockWriteBuffer();
    int bytes = send(m_fd, (const char*)m_writeBuffer->GetBufferOffset(), m_writeBuffer->GetSize(), 0);

    if(bytes < 0)
        Disconnect();
    else
        m_writeBuffer->Remove(bytes);
    UnlockWriteBuffer();

}


bool TcpSocket::Write(const void * data, size_t bytes)
{
    if(!m_connected){
        std::cout<<"TcpSocket Write return because it disconnect"<<"\n";
        return false;
    }

    bool rv = m_writeBuffer->Write(data, bytes);
    if(rv)
    {
        if(!m_writeLock)
        {
            //一个简单的等待锁, 这个Volatile变量在EPOLL里做减, 直到减到0, 说明EPOLL已经把这个socket的上次写入动作完成了, socket才再次通知EPOLL进行下次写入
            ++m_writeLock;
            sSocketEngine.WantWrite(this);
        }

    }

    return rv;
}

bool TcpSocket::Writable()
{
    return (m_writeBuffer->GetSize() > 0) ? true : false;
}

void TcpSocket::Disconnect()
{

    sSocketEngine.RemoveSocket(this);
    m_connected = false;
    //shutdown(m_fd, SD_BOTH);
    //closesocket(m_fd);
    SocketOps::CloseSocket(m_fd);
    if(!m_deleted)
        Delete();
    OnDisconnect();
}

void TcpSocket::Delete()
{

    sSocketDeleter.Add(this);
    if(m_connected) Disconnect();
}

void TcpSocket::OnError(int errcode)
{
    /* Bleh.. :p */
    std::cout<<"on err"<<"\n";
    Disconnect();
}




