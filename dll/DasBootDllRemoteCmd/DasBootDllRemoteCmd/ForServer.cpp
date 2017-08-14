#include "stdafx.h"
#include "DllCommon.h"

#include "Resource.h"
#include "DasBootDllRemoteCmd.h"

#include "ClientOperationSubWindowRemoteCmd.h"

extern "C" {
  __declspec(dllexport) int DBLoadDLLServer(std::unordered_map<std::string,
    pfnDBExportServerOperationConfirm>& mapConfirmFunctions,
    std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
    std::unordered_map<std::string, std::string>& mapDllIntroduction);
}

#define DLL_NAME _T("DasBootDllRemoteCmd.dll")
#define DLL_INTRODUCTION _T("远程控制台。")

int DBRemoteCmdOperationConfirm(SOCKET hSocket,
  CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext,
  pfnSendMessageOut pfnSMO);

static int DBDllStartRemoteCmdM(const char *strParam, SOCKET hSocket, 
  int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
static int DBDllRemoteCmdMsgM(const char *strParam, SOCKET hSocket, 
  int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
static int DBDllRemoteCmdSendM(const char *strParam, SOCKET hSocket,
  int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
static int DBDllCloseRemoteCmdM(const char *strParam, SOCKET hSocket, 
  int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);

//所有的dll必须有，并且导出此函数
int DBLoadDLLServer(std::unordered_map<std::string,
  pfnDBExportServerOperationConfirm>& mapConfirmFunctions,
  std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
  std::unordered_map<std::string, std::string>& mapDllIntroduction)
{
  mapDllIntroduction[DLL_NAME] = DLL_INTRODUCTION;
  mapConfirmFunctions[DLL_NAME] = (pfnDBExportServerOperationConfirm)DBRemoteCmdOperationConfirm;

  mapFunctions["StartRemoteCmdR"] = (pfnDBExportServer)DBDllStartRemoteCmdM;
  mapFunctions["RemoteCmdMsg00N"] = (pfnDBExportServer)DBDllRemoteCmdMsgM;
  mapFunctions["RmotCmdSendCmdR"] = (pfnDBExportServer)DBDllRemoteCmdSendM;
  mapFunctions["CloseRemoteCmdR"] = (pfnDBExportServer)DBDllCloseRemoteCmdM;

  return 0;
}

extern CDasBootDllRemoteCmdApp theApp;

int DBRemoteCmdOperationConfirm(SOCKET hSocket,
  CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext,
  pfnSendMessageOut pfnSMO)
{
  HINSTANCE hOldInst = AfxGetResourceHandle();
  AfxSetResourceHandle(theApp.m_hInstance);

  CClientOperationSubWindowRemoteCmd *pClientOperationSubWindow =
    new CClientOperationSubWindowRemoteCmd;

  if (pClientContext->m_mapClientOperationSubWindows[DLL_NAME] != NULL)
  {
    delete pClientContext->m_mapClientOperationSubWindows[DLL_NAME];
    pClientContext->m_mapClientOperationSubWindows[DLL_NAME] = NULL;
  }

  pClientContext->m_mapClientOperationSubWindows[DLL_NAME] = 
    pClientOperationSubWindow;


  pClientOperationSubWindow->Create(MAKEINTRESOURCE(IDD_DIALOG_DLL_REMOTE_CMD),
    (CWnd *)pParentWnd);

  pClientOperationSubWindow->ShowWindow(SW_SHOW);
  pClientOperationSubWindow->m_hSocket = hSocket;
  pClientOperationSubWindow->m_pfnSMO = pfnSMO;

  CString strWindowCaption;
  strWindowCaption.Format(_T("%s %d"), _T("客户控制分窗口 远程cmd 客户socket："),
    (int)hSocket);
  pClientOperationSubWindow->SetWindowText(strWindowCaption);

  AfxSetResourceHandle(hOldInst);

  return 0;
}

int DBDllStartRemoteCmdM(const char *strParam, SOCKET hSocket, int nMsgLength, 
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  //暂时什么都不做
  //done 可以考虑加载界面后，“发送命令”按钮默认不激活，收到此消息后，再激活“发送命令”按钮

  CClientOperationSubWindowRemoteCmd *pClientOperationSubWindow =
    (CClientOperationSubWindowRemoteCmd *)pClientContext->
    m_mapClientOperationSubWindows[DLL_NAME];

  CWnd *pWndTmp = pClientOperationSubWindow->
    GetDlgItem(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_SEND_COMMAND);
  pWndTmp->EnableWindow(TRUE);

  pWndTmp = pClientOperationSubWindow->
    GetDlgItem(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_INIT_REMOTE_CMD);
  pWndTmp->EnableWindow(FALSE);

  AfxMessageBox(_T("客户端cmd已激活"));

  return 0;
}

int DBDllCloseRemoteCmdM(const char *strParam, SOCKET hSocket, int nMsgLength,
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  //暂时什么都不做

  return 0;
}

int DBDllRemoteCmdSendM(const char *strParam, SOCKET hSocket, int nMsgLength,
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  //暂时什么都不做
  
  return 0;
}

//客户端pipe有新内容，并且传了过来
int DBDllRemoteCmdMsgM(const char *strParam, SOCKET hSocket, int nMsgLength,
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  CClientOperationSubWindowRemoteCmd *pClientOperationSubWindow = 
    (CClientOperationSubWindowRemoteCmd *)pClientContext->
    m_mapClientOperationSubWindows[DLL_NAME];

  CString strCmdContent;
  strCmdContent.Format("%s", strParam);

  pClientOperationSubWindow->m_edtctlStrConsoleContent += strCmdContent;

  pClientOperationSubWindow->UpdateData(FALSE);

  return 0;
}