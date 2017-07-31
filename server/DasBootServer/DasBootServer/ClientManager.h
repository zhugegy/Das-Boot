#pragma once
#include "BufPacket.h"

typedef BOOL(*PFNTransferMsg)(SOCKET s, CString& str);

#define RECV_BYTES 256 * 1024

//表示客户端上下文
struct CClientContext
{
  CClientContext(SOCKET s, sockaddr_in& addr)
  {
    m_sClient = s;
    memcpy(&m_addr, &addr, sizeof(sockaddr_in));
  }

  BOOL DoSomething(PFNTransferMsg pfn, CString& str)
  {
    if (pfn != NULL)
    {
      return pfn(m_sClient, str);
    }

    return FALSE;
  }

  SOCKET  m_sClient; //表示客户端的socket
  sockaddr_in m_addr;//表示客户端的ip和端口
  DWORD     m_dwTick;  //表示客户端最后一次更新时间
  CBufPacket m_Packet; //处理增删改查（每个client拥有自己的BufPacket，用于存储接收的数据）
                    //发送的时候，由于数据量小，所以没必要给每个client单独建发送pkt
  char      m_szRecvBuf[RECV_BYTES]; //用于每次接收(recv()函数)的临时缓冲区
};

class CClientManager
{
public:
  CClientManager();
  virtual ~CClientManager();

  //增
  BOOL Add(SOCKET s, CClientContext* pClient);

  //删
  BOOL Delete(SOCKET s);

  //查
  BOOL FindAt(SOCKET s, CClientContext*& pClient);

  BOOL IterateAndDoSomething(PFNTransferMsg pfn, CString& str);

private:
  CMap<SOCKET, SOCKET&, CClientContext*, CClientContext*> m_Map;
};

