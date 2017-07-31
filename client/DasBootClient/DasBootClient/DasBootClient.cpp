#include <string>
#include <list>
#include <unordered_map>
#include <windows.h>
// #include <WinSock2.h>
// #include <stdio.h>
#include "BufPacket.h"
#include "DasBootSocket.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib");

typedef int(*pfnDBExport)(string &strParam, list<string>& lstMsgToSend);

typedef int(*pfnPluginInterface)(unordered_map<string, pfnDBExport>& mapFunctions);

int SendMsgText(SOCKET hSocket);
DWORD RecvThreadFunc(LPVOID lpParam);


int main()
{
//   printf("helloworld\r\n");
// 
//   HINSTANCE hDll = LoadLibrary("DasBootDllProcess.dll");
// 
//   if (hDll == NULL)
//   {
//     //TRACE(_T("DLL加载失败"));
// 
//     return 0;
//   }
// 
//   pfnPluginInterface pfnInterface = (pfnPluginInterface)GetProcAddress(hDll,
//     "dbplugin_load_dll");
// 
//   unordered_map<string, pfnDBExport> mapFunctions;
// 
//   pfnInterface(mapFunctions);
// 
//   pfnDBExport pfnHelloWorld;
// 
//   pfnHelloWorld = mapFunctions["EnumProcess00000"];
// 
//   list <string> lstMsg;
// 
//   string tmp = "你好";
// 
//   pfnHelloWorld(tmp, lstMsg);
// 
//   getchar();

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

  SOCKET hSocketClient = INVALID_SOCKET;
  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));

  hSocketClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (hSocketClient == INVALID_SOCKET)
  {
    return 0;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(8889);
  addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

  int nRet = connect(hSocketClient, (sockaddr*)&addr, sizeof(sockaddr_in));

  if (nRet == SOCKET_ERROR)
  {
    return 0;
  }

  HANDLE hRecvThread = CreateThread(NULL, 0, 
    (LPTHREAD_START_ROUTINE)RecvThreadFunc, (LPVOID)hSocketClient, 0, NULL);

  SendMsgText(hSocketClient);

//   closesocket(hSocketClient);
//   WSACleanup();

  WaitForSingleObject(hRecvThread, INFINITE);

  return 0;
}

int SendMsgText(SOCKET hSocket)
{
  //开始发送数据
  CDasBootSocket sendSkt(hSocket);
  CBufPacket pkt;
  char szHead[5] = { 0 };
  DWORD dwSize = sizeof(DWORD) + sizeof(BYTE) + 6/*sizeof(DWORD) + sizeof(BYTE) + sizeof(DWORD) + sizeof(DWORD)
    + 88*/;
  (DWORD&)*szHead = dwSize;
  szHead[4] = 'a';
  pkt.Append(szHead, sizeof(szHead));
//   pkt.Append((char*)&nWidth, sizeof(DWORD));
//   pkt.Append((char*)&nHeigth, sizeof(DWORD));
//   pkt.Append(szBit, dwCount);

  pkt.Append("hello", sizeof("hello"));

  sendSkt.SendPkt(pkt);


//   if (szBit != NULL)
//   {
//     delete[] szBit;
//   }


  return 0;
}

//处理服务器发送的数据
DWORD RecvThreadFunc(LPVOID lpParam)
{
//   if (theApp.m_pMainWnd == NULL)
//   {
//     return 0;
//   }

  SOCKET sRecv = (SOCKET)lpParam;
//  CClientDlg* pDlg = (CClientDlg*)theApp.m_pMainWnd;

  while (TRUE)
  {
    CBufPacket RecvPkt;
    CDasBootSocket recvSkt(sRecv);

    int nRet = recvSkt.RecvPkt(RecvPkt);

    if (nRet == SOCKET_SUCCESS)
    {
      //开始处理接收的事件
      //lock();
      //m_dwTick = GetTickCount();
      //unlock();

      char* pszBuf = RecvPkt.GetBuf();
      switch (pszBuf[4])
      {
      case 'a':
      {
        //发送下一帧
        //pDlg->SendScreenMsg(pDlg->m_Client);
        Sleep(10);
      }
      break;

      }
    }
    else
    {
      //断开连接,释放资源

    }

  }


  return 0;
}

