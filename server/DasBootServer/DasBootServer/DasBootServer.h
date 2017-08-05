
// DasBootServer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "ClientManager.h"
#include "BufPacket.h"

// CDasBootServerApp:
// See DasBootServer.cpp for the implementation of this class
//

typedef int(*pfnSendMessageOut)(SOCKET hSocket, const char * szType, const char * szContent, DWORD dwContentLength);

typedef int(*pfnDBExportServerOperationConfirm)(SOCKET hSocket, CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);

typedef int(*pfnDBExportServer)(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);

typedef int(*pfnPluginInterfaceServer)(std::unordered_map<std::string, 
  pfnDBExportServerOperationConfirm>& mapConfirmFunctions, 
  std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
  std::unordered_map<std::string, std::string>& mapDllIntroduction);



class CDasBootServerApp : public CWinApp
{
public:
	CDasBootServerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

private:
  int m_nSocketEventNum;
  WSAEVENT m_haryEvent[WSA_MAXIMUM_WAIT_EVENTS];
  SOCKET m_harySocket[WSA_MAXIMUM_WAIT_EVENTS];

  int IncreaseSocketEventNum();
  BOOL SetSocket(int nIndex, SOCKET hSocket);
  BOOL SetEvent(int nIndex, WSAEVENT hEvent);
public:
  int GetSocketEventNum();
  int DecreaseSocketEventNum();

  BOOL AddSocketEvent(SOCKET hSocket, WSAEVENT hEvent);
  BOOL DestorySocketEvent(int nIndex);
  WSAEVENT FAR * GetEventAry();
  SOCKET GetSocketFromSocketevent(int nIndex);
  WSAEVENT GetEventFromSocketevent(int nIndex);

  CClientManager m_objClientManager;

  CList<SOCKET, SOCKET> m_hSendListSockets;
  CList<CBufPacket *, CBufPacket *> m_hSendListPkts;
  int m_nSendListElementCount;
  CRITICAL_SECTION m_csSendListOperation;

  std::unordered_map<std::string, pfnDBExportServerOperationConfirm> m_mapConfirmFunctions;
  std::unordered_map<std::string, std::string> m_mapDllIntroduction;
  CList<CString, CString &> m_lstCurrentDllNames;
};

extern CDasBootServerApp theApp;