#pragma once

#include "stdafx.h"
#include <string>
#include <unordered_map>

class CClientOperationMainWindow;
class CClientContext;

#define MSG_ELEMENT_DELIM '\1'

typedef int(*pfnSendMessageOut)(SOCKET hSocket, const char * szType, 
  const char * szContent, DWORD dwContentLength);

typedef int(*pfnDBExportClient)(const char *strParam, int nMsgLength, 
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO);
typedef int(*pfnPluginInterfaceClient)(std::unordered_map<std::string, 
  pfnDBExportClient>& mapFunctions);

typedef int(*pfnDBExportServerOperationConfirm)(SOCKET hSocket, 
  CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext, 
  pfnSendMessageOut pfnSMO);
typedef int(*pfnDBExportServer)(const char *strParam, SOCKET hSocket, 
  int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
typedef int(*pfnPluginInterfaceServer)(std::unordered_map<std::string, 
  pfnDBExportServerOperationConfirm>& mapConfirmFunctions, 
  std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
  std::unordered_map<std::string, std::string>& mapDllIntroduction);

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
  void operator=(CBufPacket& pkt);

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

typedef BOOL(*PFNTransferMsg)(SOCKET s, CString& str);

#define RECV_BYTES 256 * 1024

//表示客户端上下文
struct CClientContext
{
  CClientContext(SOCKET s, sockaddr_in& addr)
  {
    m_sClient = s;
    memcpy(&m_addr, &addr, sizeof(sockaddr_in));

    m_pMyClientOperationMainWindow = NULL;
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

  FILE *m_pCurrentFile = NULL;
  int m_nFileChunckCount = 0;

  CClientOperationMainWindow *m_pMyClientOperationMainWindow;
  std::unordered_map<std::string, CDialog*> m_mapClientOperationSubWindows;
};

/*
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
};*/

