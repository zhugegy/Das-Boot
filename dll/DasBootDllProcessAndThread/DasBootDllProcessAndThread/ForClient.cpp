#include "stdafx.h"

#include "DllCommon.h"

#include <windows.h>
#include <Tlhelp32.h>

extern "C" {
  __declspec(dllexport) int DBLoadDLLClient(std::unordered_map<std::string, pfnDBExportClient>& mapFunctions);
}

int DBDllRefreshProcessR(const char *strParam, int nMsgLength, 
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO);
int DBDllTerminateProcessR(const char *strParam, int nMsgLength,
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO);

//所有的dll必须有，并且导出此函数
int DBLoadDLLClient(std::unordered_map<std::string, pfnDBExportClient>& mapFunctions)
{
  mapFunctions["RefreshProcessC"] = (pfnDBExportClient)DBDllRefreshProcessR;
  mapFunctions["TerminateProceC"] = (pfnDBExportClient)DBDllTerminateProcessR;


  return 0;
}

int DBDllRefreshProcessR(const char *strParam, int nMsgLength,
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  HANDLE hSnapshort = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  PROCESSENTRY32 pe32 = { 0 };
  pe32.dwSize = sizeof(PROCESSENTRY32);

  CString strProcessListInfo;
  CString strProcessInstanceInfo;

  if (Process32First(hSnapshort, &pe32))
  {
    do
    {
      strProcessInstanceInfo.Format("%s%c%d%c%d%c%d%c", 
        pe32.szExeFile, MSG_ELEMENT_DELIM,
        pe32.th32ProcessID, MSG_ELEMENT_DELIM,
        pe32.cntThreads, MSG_ELEMENT_DELIM,
        pe32.th32ParentProcessID, MSG_ELEMENT_DELIM);

      strProcessListInfo.Append(strProcessInstanceInfo);

    } while (Process32Next(hSnapshort, &pe32));

  }

  CloseHandle(hSnapshort);

  pfnSMO(hSocketClient, TEXT("RefreshProcessR"), 
    strProcessListInfo.GetBuffer(0), -1);

  return 0;
}

int DBDllTerminateProcessR(const char *strParam, int nMsgLength,
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  CString strReply;

  HANDLE hProcess = INVALID_HANDLE_VALUE;
  DWORD dwPID = (DWORD)atoi(strParam);

  hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);

  if (hProcess != INVALID_HANDLE_VALUE)
  {
    TerminateProcess(hProcess, 0);
    strReply.Format("%s%c%s", strParam, MSG_ELEMENT_DELIM, _T("SUCCESS"));
    pfnSMO(hSocketClient, TEXT("TerminateProceR"),
      strReply.GetBuffer(0), -1);
  }
  else
  {
    strReply.Format("%s%c%s", strParam, MSG_ELEMENT_DELIM, _T("FAILURE"));
    pfnSMO(hSocketClient, TEXT("TerminateProceR"),
      strReply.GetBuffer(0), -1);
  }

  CloseHandle(hProcess);
  
  return 0;
}