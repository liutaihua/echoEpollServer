//#include "Network.h"
#include  <stddef.h>

class BaseBuffer;
class BaseSocket
{
public:

     bool m_WillDisconnet;
	virtual ~BaseSocket() {}
    inline int GetFd() { return m_fd; }

    inline void SetFd(int new_fd) { m_fd = new_fd; }

	virtual void OnRead(size_t len) = 0;

	virtual void OnWrite(size_t len) = 0;

    virtual bool Writable() = 0;
	virtual void OnError(int errcode) = 0;

	virtual void OnAccept(void * pointer) = 0;

	inline bool IsConnected() { return m_connected; }

	volatile long m_writeLock;

	virtual void Disconnect() = 0;

	virtual void Delete() = 0;

protected:
    int m_fd;

	bool m_deleted;
	bool m_connected;

    BaseBuffer* m_readBuffer;

    BaseBuffer* m_writeBuffer;

};

