#include "stdafx.h"

#include "DllCommon.h"
#include "resource.h"	
#include "DasBootDllHelloWorld.h"

#include "ClientOperationSubWindowHelloWorld.h"

int DBHelloWorldOperationConfirm(SOCKET hSocket, CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
int DBDllHelloWorldM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);

extern "C" {
  __declspec(dllexport) int DBLoadDLLServer(std::unordered_map<std::string,
    pfnDBExportServerOperationConfirm>& mapConfirmFunctions,
    std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
    std::unordered_map<std::string, std::string>& mapDllIntroduction);
}

#define DLL_NAME _T("DasBootDllHelloWorld.dll")
#define DLL_INTRODUCTION _T("HelloWorld展示程序（dll模板）。")

//所有的dll必须有，并且导出此函数
int DBLoadDLLServer(std::unordered_map<std::string, 
  pfnDBExportServerOperationConfirm>& mapConfirmFunctions, 
  std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
  std::unordered_map<std::string, std::string>& mapDllIntroduction)
{
  mapConfirmFunctions[DLL_NAME] = (pfnDBExportServerOperationConfirm)DBHelloWorldOperationConfirm;
  mapFunctions["HelloWorld0000R"] = (pfnDBExportServer)DBDllHelloWorldM;
  mapDllIntroduction[DLL_NAME] = DLL_INTRODUCTION;

  return 0;
}

extern CDasBootDllHelloWorldApp theApp;

int DBHelloWorldOperationConfirm(SOCKET hSocket, CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  HINSTANCE hOldInst = AfxGetResourceHandle();
  AfxSetResourceHandle(theApp.m_hInstance);

  CClientOperationSubWindowHelloWorld *pClientOperationSubWindow =
    new CClientOperationSubWindowHelloWorld;

  if (pClientContext->m_mapClientOperationSubWindows[DLL_NAME] != NULL)
  {
    delete pClientContext->m_mapClientOperationSubWindows[DLL_NAME];
    pClientContext->m_mapClientOperationSubWindows[DLL_NAME] = NULL;
  }

  pClientContext->m_mapClientOperationSubWindows[DLL_NAME] = pClientOperationSubWindow;


  pClientOperationSubWindow->Create(MAKEINTRESOURCE(IDD_DIALOG_DLL_HELLO_WORLD), (CWnd *)pParentWnd);

  pClientOperationSubWindow->ShowWindow(SW_SHOW);
  pClientOperationSubWindow->m_hSocket = hSocket;
  pClientOperationSubWindow->m_pfnSMO = pfnSMO;

  CString strWindowCaption;
  strWindowCaption.Format(_T("%s %d"), _T("客户控制分窗口 HelloWorld 客户socket："),
    (int)hSocket);
  pClientOperationSubWindow->SetWindowText(strWindowCaption);

  AfxSetResourceHandle(hOldInst);

  return 0;
}

int DBDllHelloWorldM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  CString strRecvdMsg;
  strRecvdMsg.Format(_T("%s"), strParam);

  CClientOperationSubWindowHelloWorld *pClientOperationSubWindow = 
    (CClientOperationSubWindowHelloWorld *)pClientContext->m_mapClientOperationSubWindows[DLL_NAME];

  //如果窗口不存在（已被关闭），则直接退出，避免程序（可能发生的）崩溃
  if (pClientOperationSubWindow == NULL)
  {
    return 0;
  }

  pClientOperationSubWindow->m_strEdit.Append(strRecvdMsg);

  pClientOperationSubWindow->UpdateData(FALSE);


  return 0;
}