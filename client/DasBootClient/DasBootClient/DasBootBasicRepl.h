#pragma once
#include <afxtempl.h>
#include <afxsock.h>

int SendMessageOut(SOCKET hSocket, const char * szType, const char * szContent, DWORD dwContentLength = -1);


int AddBasicReplToMapFunctions();


