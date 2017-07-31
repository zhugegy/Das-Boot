#pragma once
#include <windows.h>

//缓冲区，所有想要发送的数据，都先来到缓冲区暂时存放
class CBufPacket
{
public:
#define BUF_SIZE 256
#define BUF_FACTOR 2
#define WM_PKTHANDLEMSG WM_USER + 1

public:
  CBufPacket();
  virtual ~CBufPacket();

  CBufPacket(CBufPacket& pkt);

  BOOL InitBuf();
  BOOL ReleaseBuf();

  BOOL SetBuf(char* pszBuf, int nBufSize, int nLength);

  char* GetBuf(); //获取缓冲区
  int GetLength(); //获取缓冲区长度

  BOOL Append(char* pBuf, int nLength);//将数据添加到缓冲区中

  BOOL Handle(SOCKET s);  //自处理

private:
  char* m_pchBuf;    //缓冲区
  int   m_nSize;   //缓冲区大小
  int   m_nLength; //缓冲区中数据的实际长度
};

