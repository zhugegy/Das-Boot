#include "stdafx.h"
#include "DllCommon.h"

#include "resource.h"	
#include "DasBootDllScreen.h"

#include "ClientOperationSubWindow.h"

extern "C" {
  __declspec(dllexport) int DBLoadDLLServer(std::unordered_map<std::string,
    pfnDBExportServerOperationConfirm>& mapConfirmFunctions,
    std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
    std::unordered_map<std::string, std::string>& mapDllIntroduction);
}

#define DLL_NAME _T("DasBootDllScreen.dll")
#define DLL_INTRODUCTION _T("远程屏幕。")

#define SERVER_MONITOR_WIDTH 800
#define SERVER_MONITOR_HEIGHT 600
#define SERVER_REFRESH_TIME_INTERVAL 3000  //微秒为单位

int DBScreenOperationConfirm(SOCKET hSocket, 
  CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext, 
  pfnSendMessageOut pfnSMO);

int DBDllGetScreenSizeM(const char *strParam, SOCKET hSocket, int nMsgLength,
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
int DBDllCaptureScreenM(const char *strParam, SOCKET hSocket, int nMsgLength,
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO);

DWORD RefreshMonitorThread(LPARAM lparam);

static char** str_split(char* a_str, const char a_delim);


//所有的dll必须有，并且导出此函数
int DBLoadDLLServer(std::unordered_map<std::string,
  pfnDBExportServerOperationConfirm>& mapConfirmFunctions,
  std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
  std::unordered_map<std::string, std::string>& mapDllIntroduction)
{
  mapDllIntroduction[DLL_NAME] = DLL_INTRODUCTION;
  mapConfirmFunctions[DLL_NAME] = 
    (pfnDBExportServerOperationConfirm)DBScreenOperationConfirm;

  mapFunctions["GetScreenSize0R"] = (pfnDBExportServer)DBDllGetScreenSizeM;
  mapFunctions["CaptureScreen0R"] = (pfnDBExportServer)DBDllCaptureScreenM;

  return 0;
}

extern CDasBootDllScreenApp theApp;

int DBScreenOperationConfirm(SOCKET hSocket, 
  CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext, 
  pfnSendMessageOut pfnSMO)
{
  HINSTANCE hOldInst = AfxGetResourceHandle();
  AfxSetResourceHandle(theApp.m_hInstance);

  CClientOperationSubWindow *pClientOperationSubWindow =
    new CClientOperationSubWindow;

  if (pClientContext->m_mapClientOperationSubWindows[DLL_NAME] != NULL)
  {
    delete pClientContext->m_mapClientOperationSubWindows[DLL_NAME];
    pClientContext->m_mapClientOperationSubWindows[DLL_NAME] = NULL;
  }

  pClientContext->m_mapClientOperationSubWindows[DLL_NAME] = 
    pClientOperationSubWindow;


  pClientOperationSubWindow->Create(MAKEINTRESOURCE(IDD_DIALOG_DLL_SCREEN),
    (CWnd *)pParentWnd);

  pClientOperationSubWindow->ShowWindow(SW_SHOW);
  pClientOperationSubWindow->m_hSocket = hSocket;
  pClientOperationSubWindow->m_pfnSMO = pfnSMO;

  CString strWindowCaption;
  strWindowCaption.Format(_T("%s %d"), _T("客户控制分窗口 远程屏幕 客户socket："),
    (int)hSocket);
  pClientOperationSubWindow->SetWindowText(strWindowCaption);

  AfxSetResourceHandle(hOldInst);

  return 0;
}


int DBDllGetScreenSizeM(const char *strParam, SOCKET hSocket, int nMsgLength,
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  CString strRecvdMsg;
  strRecvdMsg.Format(_T("%s"), strParam);

  CClientOperationSubWindow *pClientOperationSubWindow =
    (CClientOperationSubWindow *)pClientContext->
    m_mapClientOperationSubWindows[DLL_NAME];

  //如果窗口不存在（已被关闭），则直接退出，避免程序（可能发生的）崩溃
  if (pClientOperationSubWindow == NULL)
  {
    return 0;
  }

  char** tokens;
  tokens = str_split((char *)strParam, MSG_ELEMENT_DELIM);

  if (tokens)
  {
    for (int j = 0; *(tokens + j); j++)
    {
      if (j == 0)
      {
        pClientOperationSubWindow->m_nScreenWidth = atoi(*(tokens + j));
      }
      
      if (j == 1)
      {
        pClientOperationSubWindow->m_nScreenHeight = atoi(*(tokens + j));
      }

      free(*(tokens + j));
    }
    free(tokens);
  }//if (tokens)

  //AfxMessageBox(_T("初始化成功//已获取客户端屏幕分辨率"));
  pfnSMO(hSocket, _T("CaptureScreen0C"), _T("nothing"), -1);

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
      //assert(idx < count);
      *(result + idx++) = strdup(token);
      token = strtok(0, delim);
    }
    //assert(idx == count - 1);
    *(result + idx) = 0;
  }

  return result;
}

int DBDllCaptureScreenM(const char *strParam, SOCKET hSocket, int nMsgLength,
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  CClientOperationSubWindow *pClientOperationSubWindow =
    (CClientOperationSubWindow *)pClientContext->
    m_mapClientOperationSubWindows[DLL_NAME];

  //如果窗口不存在（已被关闭），则直接退出，避免程序（可能发生的）崩溃
  if (pClientOperationSubWindow == NULL)
  {
    return 0;
  }

  CDC memDC;
  CDC* pDC = pClientOperationSubWindow->GetDC();

  memDC.CreateCompatibleDC(pDC);
  CBitmap bitMap;
  bitMap.CreateCompatibleBitmap(pDC, pClientOperationSubWindow->m_nScreenWidth, 
    pClientOperationSubWindow->m_nScreenHeight);

  bitMap.SetBitmapBits(4 * pClientOperationSubWindow->m_nScreenWidth * 
    pClientOperationSubWindow->m_nScreenHeight, strParam);

  memDC.SelectObject(&bitMap);

  BOOL bRet = pDC->BitBlt(0, 0, SERVER_MONITOR_WIDTH, SERVER_MONITOR_HEIGHT, 
    &memDC, 0, 0, SRCCOPY);

  if (bRet == FALSE)
  {
    return 0;
  }

  if (pClientOperationSubWindow->m_bIsMonitoring == true)
  {
    HANDLE hSendThread = CreateThread(NULL, 0,
      (LPTHREAD_START_ROUTINE)RefreshMonitorThread, (LPVOID)pClientContext, 0,
      NULL);
  }

  return 0;
}


DWORD RefreshMonitorThread(LPARAM lparam)
{
  //这里有点绕远路的感觉，但是是为了省力（lparam传参）
  CClientContext* pClientContext = (CClientContext*)lparam;

  Sleep(SERVER_REFRESH_TIME_INTERVAL);

  CClientOperationSubWindow *pClientOperationSubWindow =
    (CClientOperationSubWindow *)pClientContext->
    m_mapClientOperationSubWindows[DLL_NAME];

  pClientOperationSubWindow->m_pfnSMO(pClientOperationSubWindow->m_hSocket, 
    _T("CaptureScreen0C"), _T("nothing"), -1);

  return 0;
}