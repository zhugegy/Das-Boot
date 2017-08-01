#pragma once

int SendMessageOut(SOCKET hSocket, const char * szType, const char * szContent);
int AddBasicCommandsToMapFunctions();
int DBBCClientListInfoQueryC(SOCKET hSocket);