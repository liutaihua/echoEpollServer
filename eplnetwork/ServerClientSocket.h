#include "Network.h"
#include <queue>
//#include "DataClientSocket.h"

class SClientSocket;

typedef struct STNetMsgHeader
{
    uint32	dwMsgID;			// 包头标示
    uint32	dwDataLen;		// 数据包长度(不包括包头)
} STNetMsgHeader;


class SClientSocket : public TcpSocket
{
public:
  SClientSocket(SOCKET fd, const sockaddr_in * peer);
  ~SClientSocket();

//  void OnRecvData();
//  void OnDisconnect();
//  void OnConnect();

  virtual void OnRecvData();
  virtual void OnConnect();
  virtual void OnDisconnect();

};
