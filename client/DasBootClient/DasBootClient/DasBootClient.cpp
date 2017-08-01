#include <string>
#include <list>
#include <unordered_map>
#include <windows.h>
#include "BufPacket.h"
#include "DasBootSocket.h"
#include "DasBootBasicRepl.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")

typedef int(*pfnDBExportClient)(const char *strParam);

typedef int(*pfnPluginInterfaceClient)(unordered_map<string, pfnDBExportClient>& mapFunctions);

int SendMessageOut(SOCKET hSocket, const char * szType, const char * szContent);
DWORD RecvThreadFunc(LPVOID lpParam);

//全局变量
unordered_map<string, pfnDBExportClient> g_mapFunctions;
SOCKET g_hSocketClient = INVALID_SOCKET;

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

  //初始化map
  AddBasicReplToMapFunctions();

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

      printf(TEXT("%s\r\n"), pszBuf + 4);  //debug
      printf(TEXT("%s\r\n"), pszBuf + 4 + 16);  //debug

      char szMsgType[16] = { 0 };
      strcpy(szMsgType, pszBuf + 4);

      pfnDBExportClient pfn;
      pfn = g_mapFunctions[szMsgType];
      pfn(pszBuf + 4 + 16);
    }
    else
    {
      //断开连接,释放资源

    }

  }


  return 0;
}

