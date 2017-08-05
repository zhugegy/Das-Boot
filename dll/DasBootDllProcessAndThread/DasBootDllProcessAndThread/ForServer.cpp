#include "stdafx.h"
#include "DllCommon.h"

#include "Resource.h"
#include "DasBootDllProcessAndThread.h"
#include "ClientOperationSubWindowProcessAndThread.h"

extern "C" {
  __declspec(dllexport) int DBLoadDLLServer(std::unordered_map<std::string,
    pfnDBExportServerOperationConfirm>& mapConfirmFunctions,
    std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
    std::unordered_map<std::string, std::string>& mapDllIntroduction);
}

#define DLL_NAME _T("DasBootDllProcessAndThread.dll")
#define DLL_INTRODUCTION _T("操作目标客户端的进程和线程。")

int DBProcessAndThreadOperationConfirm(SOCKET hSocket,
  CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext,
  pfnSendMessageOut pfnSMO);

int DBDllRefreshProcessM(const char *strParam, SOCKET hSocket, int nMsgLength, 
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
int DBDllTerminateProcessM(const char *strParam, SOCKET hSocket, int nMsgLength,
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO);

static char** str_split(char* a_str, const char a_delim);

//所有的dll必须有，并且导出此函数
int DBLoadDLLServer(std::unordered_map<std::string,
  pfnDBExportServerOperationConfirm>& mapConfirmFunctions,
  std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
  std::unordered_map<std::string, std::string>& mapDllIntroduction)
{
  mapDllIntroduction[DLL_NAME] = DLL_INTRODUCTION;
  mapConfirmFunctions[DLL_NAME] = (pfnDBExportServerOperationConfirm)DBProcessAndThreadOperationConfirm;
  mapFunctions["RefreshProcessR"] = (pfnDBExportServer)DBDllRefreshProcessM;
  mapFunctions["TerminateProceR"] = (pfnDBExportServer)DBDllTerminateProcessM;


  return 0;
}

extern CDasBootDllProcessAndThreadApp theApp;

int DBProcessAndThreadOperationConfirm(SOCKET hSocket, 
  CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext, 
  pfnSendMessageOut pfnSMO)
{
  HINSTANCE hOldInst = AfxGetResourceHandle();
  AfxSetResourceHandle(theApp.m_hInstance);

  CClientOperationSubWindowProcessAndThread *pClientOperationSubWindow =
    new CClientOperationSubWindowProcessAndThread;

  if (pClientContext->m_mapClientOperationSubWindows[DLL_NAME] != NULL)
  {
    delete pClientContext->m_mapClientOperationSubWindows[DLL_NAME];
    pClientContext->m_mapClientOperationSubWindows[DLL_NAME] = NULL;
  }

  pClientContext->m_mapClientOperationSubWindows[DLL_NAME] = pClientOperationSubWindow;


  pClientOperationSubWindow->Create(MAKEINTRESOURCE(IDD_DIALOG_DLL_PROCESS_AND_THREAD), (CWnd *)pParentWnd);

  pClientOperationSubWindow->ShowWindow(SW_SHOW);
  pClientOperationSubWindow->m_hSocket = hSocket;
  pClientOperationSubWindow->m_pfnSMO = pfnSMO;

  CString strWindowCaption;
  strWindowCaption.Format(_T("%s %d"), _T("客户控制分窗口 进程和线程 客户socket："),
    (int)hSocket);
  pClientOperationSubWindow->SetWindowText(strWindowCaption);

  AfxSetResourceHandle(hOldInst);

  return 0;
}

int DBDllRefreshProcessM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  CClientOperationSubWindowProcessAndThread *pClientOperationSubWindow = (CClientOperationSubWindowProcessAndThread *)pClientContext->m_mapClientOperationSubWindows[DLL_NAME];

  //如果窗口不存在（已被关闭），则直接退出，避免程序（可能发生的）崩溃
  if (pClientOperationSubWindow == NULL)
  {
    return 0;
  }

  pClientOperationSubWindow->m_lstctlProcessList.DeleteAllItems();

  //TRACE("%s", strParam);

  char** tokens;
  tokens = str_split((char *)strParam, MSG_ELEMENT_DELIM);

  if (tokens)
  {
    for (int j = 0; *(tokens + j); j++)
    {
      if (j % 4 == 0)
      {
        //zgc undone这里对ListControl控件的使用不是很好
        pClientOperationSubWindow->m_lstctlProcessList.InsertItem(
          LVIF_TEXT | LVIF_STATE, 0, *(tokens + j), 0, 0, 0, 0);
      }

      pClientOperationSubWindow->m_lstctlProcessList.SetItemText(0, j % 4,
        *(tokens + j));

      if (j % 4 == 1)  //如果是PID项
      {
        int nPID = atoi(*(tokens + j));
        pClientOperationSubWindow->m_lstctlProcessList.SetItemData(0, nPID);
      }

      free(*(tokens + j));
    }
    free(tokens);
  }//if (tokens)

  pClientOperationSubWindow->UpdateData(FALSE);

  return 0;
}

//https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
char** str_split(char* a_str, const char a_delim)
{
  char** result = 0;
  size_t count = 0;
  char* tmp = a_str;
  char* last_comma = 0;
  char delim[2];
  delim[0] = a_delim;
  delim[1] = 0;

  /* Count how many elements will be extracted. */
  while (*tmp)
  {
    if (a_delim == *tmp)
    {
      count++;
      last_comma = tmp;
    }
    tmp++;
  }

  /* Add space for trailing token. */
  count += last_comma < (a_str + strlen(a_str) - 1);

  /* Add space for terminating null string so caller
  knows where the list of returned strings ends. */
  count++;

  result = (char **)malloc(sizeof(char*) * count);

  if (result)
  {
    size_t idx = 0;
    char* token = strtok(a_str, delim);

    while (token)
    {
      assert(idx < count);
      *(result + idx++) = strdup(token);
      token = strtok(0, delim);
    }
    assert(idx == count - 1);
    *(result + idx) = 0;
  }

  return result;
}

int DBDllTerminateProcessM(const char *strParam, SOCKET hSocket, int nMsgLength,
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  //do nothing

//   CClientOperationSubWindowProcessAndThread *pClientOperationSubWindow = (CClientOperationSubWindowProcessAndThread *)pClientContext->m_mapClientOperationSubWindows[DLL_NAME];

//如果窗口不存在（已被关闭），则直接退出，避免程序（可能发生的）崩溃
//   if (pClientOperationSubWindow == NULL)
//   {
//     return 0;
//   }

// 
//   pClientOperationSubWindow->m_lstctlProcessList.刷新

  return 0;
}