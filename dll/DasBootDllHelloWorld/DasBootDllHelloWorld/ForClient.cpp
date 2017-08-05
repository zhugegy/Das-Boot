#include "stdafx.h"

#include "DllCommon.h"

extern "C" {
  __declspec(dllexport) int DBLoadDLLClient(std::unordered_map<std::string, pfnDBExportClient>& mapFunctions);
}

int DBDllHelloWorldR(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO);

//所有的dll必须有，并且导出此函数
int DBLoadDLLClient(std::unordered_map<std::string, pfnDBExportClient>& mapFunctions)
{
  mapFunctions["HelloWorld0000C"] = (pfnDBExportClient)DBDllHelloWorldR;

  return 0;
}

int DBDllHelloWorldR(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  pfnSMO(hSocketClient, TEXT("HelloWorld0000R"), TEXT("HelloWorld reply from client's dll. "), -1);

  return 0;
}