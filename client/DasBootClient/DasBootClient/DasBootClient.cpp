#include <string>
#include <unordered_map>
#include <afxtempl.h>
#include "DasBootClient.h"
#include "BufPacket.h"
#include "DasBootSocket.h"
#include "DasBootBasicRepl.h"
#include "LoadDll.h"

//using namespace std;

#pragma comment(lib, "ws2_32.lib")

DWORD RecvThreadFunc(LPVOID lpParam);
DWORD SendPktThreadProc(LPARAM lparam);

//全局变量
std::unordered_map<std::string, pfnDBExportClient> g_mapFunctions;
SOCKET g_hSocketClient = INVALID_SOCKET;
FILE *g_pCurrentFile = NULL;
int g_nFileChunckCount = 0;

CList<SOCKET, SOCKET> g_hSendListSockets;
CList<CBufPacket *, CBufPacket *> g_hSendListPkts;
int g_nSendListElementCount = 0;
CRITICAL_SECTION g_csSendListOperation;

int main()
{
  InitializeCriticalSection(&g_csSendListOperation);

  //载入现有dll
  LoadExsitingDllWhenStartUp();

  //初始化map
  AddBasicReplToMapFunctions();

  HANDLE hSendThread = CreateThread(NULL, 0,
    (LPTHREAD_START_ROUTINE)SendPktThreadProc, 0, 0, NULL);

  WORD wVersionRequested;
  WSADATA wsaData;
  int err;

  wVersionRequested = MAKEWORD(1, 1);
  err = WSAStartup(wVersionRequested, &wsaData);

  if (err != 0)
  {
    WSACleanup();

    return 0;
  }

  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));

  g_hSocketClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (g_hSocketClient == INVALID_SOCKET)
  {
    return 0;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(8889);
  addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

  int nRet = connect(g_hSocketClient, (sockaddr*)&addr, sizeof(sockaddr_in));

  if (nRet == SOCKET_ERROR)
  {
    return 0;
  }

  HANDLE hRecvThread = CreateThread(NULL, 0, 
    (LPTHREAD_START_ROUTINE)RecvThreadFunc, (LPVOID)g_hSocketClient, 0, NULL);

//   closesocket(hSocketClient);
//   WSACleanup();

  WaitForSingleObject(hRecvThread, INFINITE);
  TerminateThread(hSendThread, 0);

  DeleteCriticalSection(&g_csSendListOperation);

  return 0;
}

//处理服务器发送的数据
DWORD RecvThreadFunc(LPVOID lpParam)
{
  SOCKET sRecv = (SOCKET)lpParam;

  while (TRUE)
  {
    CBufPacket RecvPkt;
    CDasBootSocket recvSkt(sRecv);

    int nRet = recvSkt.RecvPkt(RecvPkt);

    if (nRet == SOCKET_SUCCESS)
    {
      char* pszBuf = RecvPkt.GetBuf();

      printf(TEXT("%s\r\n"), pszBuf + 4);  //debug
      printf(TEXT("%s\r\n"), pszBuf + 4 + 16);  //debug
      int nMsgLength = (int &)(*pszBuf);
      char szMsgType[16] = { 0 };
      strcpy(szMsgType, pszBuf + 4);

      pfnDBExportClient pfn;
      pfn = g_mapFunctions[szMsgType];
      pfn(pszBuf + 4 + 16, nMsgLength, g_hSocketClient, &SendMessageOut);
    }
    else
    {
      //断开连接,释放资源

    }

  }

  return 0;
}

DWORD SendPktThreadProc(LPARAM lparam)
{
  while (true)
  {
    if (g_nSendListElementCount != 0)
    {
      if (g_hSendListPkts.GetCount() != 0)
      {
        EnterCriticalSection(&g_csSendListOperation);

        SOCKET hSocket = g_hSendListSockets.GetTail();
        CBufPacket *pTmpPkt = g_hSendListPkts.GetTail();
        CDasBootSocket sendSkt(hSocket);
        sendSkt.SendData((*pTmpPkt).GetBuf(), (*pTmpPkt).GetLength());
        delete pTmpPkt;
        g_hSendListSockets.RemoveTail();
        g_hSendListPkts.RemoveTail();
        g_nSendListElementCount--;

        LeaveCriticalSection(&g_csSendListOperation);
      }
    }

    Sleep(50);  //30以上保险
  }

  return 0;
}