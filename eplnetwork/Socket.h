

#ifndef SOCKET_H
#define SOCKET_H
//#include "BaseSocket.h"
#include "SocketOps.h"
#include "Network.h"
//#include "Mutex.h"
#include "StraightBuffer.h"
#include <string.h>

class TcpSocket : public BaseSocket
{
public:
    TcpSocket(int fd, const sockaddr_in * peer);

    ~TcpSocket();

    bool Write(const void * data, size_t bytes);

    bool Read(void * destination, size_t bytes)
    {
        return m_readBuffer->Read(destination, bytes);
    }

    void Disconnect();
    void Finalize();

    void Delete();

    void OnRead(size_t len);

    void OnWrite(size_t len);

    virtual void OnRecvData() {}
    virtual void OnConnect() {}
    virtual void OnDisconnect() {}

    const char * GetIP() { return inet_ntoa(m_peer.sin_addr); }

    void OnError(int errcode);
    bool Writable();

    void OnAccept(void * pointer) {}

    inline void LockWriteBuffer() { m_writeMutex.Acquire(); }
    inline void UnlockWriteBuffer() { m_writeMutex.Release(); }

    inline BaseBuffer * GetReadBuffer() { return m_readBuffer; }
    inline BaseBuffer * GetWriteBuffer() { return m_writeBuffer; }
protected:

    sockaddr_in m_peer;
    Mutex m_writeMutex;
};

#endif
