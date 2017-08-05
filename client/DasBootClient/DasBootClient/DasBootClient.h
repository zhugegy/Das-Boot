#pragma once
#include <afxtempl.h>
#include <afxsock.h>
#include <string>
#include <unordered_map>

typedef int(*pfnSendMessageOut)(SOCKET hSocket, const char * szType, const char * szContent, DWORD dwContentLength);

typedef int(*pfnDBExportClient)(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO);

typedef int(*pfnPluginInterfaceClient)(std::unordered_map<std::string, pfnDBExportClient>& mapFunctions);
