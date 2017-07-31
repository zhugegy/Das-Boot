#pragma once
#include "BufPacket.h"

class CDasBootSocket
{
public:
#define SOCKET_SUCCESS 1

public:
  CDasBootSocket(SOCKET hSocket);
  virtual ~CDasBootSocket();

  int SendPkt(CBufPacket& pkt);
  int RecvPkt(CBufPacket& pkt);  //undone

  int SendData(const char* pszData, int nLength);
  int RecvData(char* pszData, int nLength);

private:
  SOCKET m_Socket;
};
