
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
};

extern CDasBootServerApp theApp;