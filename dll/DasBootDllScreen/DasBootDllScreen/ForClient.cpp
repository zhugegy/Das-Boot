#include "stdafx.h"
#include "DllCommon.h"

#include <afxwin.h>

extern "C" {
  __declspec(dllexport) int DBLoadDLLClient(
    std::unordered_map<std::string, pfnDBExportClient>& mapFunctions);
}

int DBDllCaptureScreenR(const char *strParam, int nMsgLength, 
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO);
int DBDllGetScreenSizeR(const char *strParam, int nMsgLength,
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO);

//所有的dll必须有，并且导出此函数
int DBLoadDLLClient(
  std::unordered_map<std::string, pfnDBExportClient>& mapFunctions)
{
  mapFunctions["CaptureScreen0C"] = (pfnDBExportClient)DBDllCaptureScreenR;
  mapFunctions["GetScreenSize0C"] = (pfnDBExportClient)DBDllGetScreenSizeR;

  return 0;
}

int DBDllCaptureScreenR(const char *strParam, int nMsgLength,
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  CDC memDC;
  CDC* pDesktopDC = CWnd::GetDesktopWindow()->GetDC();  //static函数

  memDC.CreateCompatibleDC(pDesktopDC);

  int nWidth = GetSystemMetrics(SM_CXSCREEN);
  int nHeigth = GetSystemMetrics(SM_CYSCREEN);

  CBitmap bitMap;
  bitMap.CreateCompatibleBitmap(pDesktopDC, nWidth, nHeigth);
  memDC.SelectObject(&bitMap);

  BOOL bRet = memDC.BitBlt(0, 0, nWidth, nHeigth, pDesktopDC, 0, 0, SRCCOPY);

  if (bRet == FALSE)
  {
    return 0;
  }

  //RGB(0,0,0);
  DWORD dwCount = 4 * nWidth * nHeigth;
  char* szBit = new char[dwCount];
  if (szBit == NULL)
  {
    return 0;
  }

  DWORD dwRet = bitMap.GetBitmapBits(dwCount, szBit);

  if (dwRet == 0)
  {
    return 0;
  }

  pfnSMO(hSocketClient, TEXT("CaptureScreen0R"), szBit, dwCount);

  return 0;
}

int DBDllGetScreenSizeR(const char *strParam, int nMsgLength,
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  int nWidth = GetSystemMetrics(SM_CXSCREEN);
  int nHeight = GetSystemMetrics(SM_CYSCREEN);

  CString strScreenSizeInfo;

  strScreenSizeInfo.Format("%d%c%d", nWidth, MSG_ELEMENT_DELIM, nHeight);

  pfnSMO(hSocketClient, TEXT("GetScreenSize0R"), strScreenSizeInfo, -1);

  return 0;
}