
// DasBootServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DasBootServer.h"
#include "DasBootServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDasBootServerApp

BEGIN_MESSAGE_MAP(CDasBootServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDasBootServerApp construction

CDasBootServerApp::CDasBootServerApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
  m_nSocketEventNum = 0;

  memset(m_harySocket, 0, sizeof(m_harySocket));
  memset(m_haryEvent, 0, sizeof(m_haryEvent));

}


// The one and only CDasBootServerApp object

CDasBootServerApp theApp;


// CDasBootServerApp initialization

BOOL CDasBootServerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CDasBootServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CDasBootServerApp::GetSocketEventNum()
{

  return m_nSocketEventNum;
}


int CDasBootServerApp::IncreaseSocketEventNum()
{
  m_nSocketEventNum++;

  return m_nSocketEventNum;
}


int CDasBootServerApp::DecreaseSocketEventNum()
{
  m_nSocketEventNum--;

  return m_nSocketEventNum;
}


BOOL CDasBootServerApp::SetSocket(int nIndex, SOCKET hSocket)
{
  if (nIndex < 0 || nIndex >= WSA_MAXIMUM_WAIT_EVENTS)
  {
    TRACE(_T("添加socket至socket数组时，超出数组的范围（0-63）"));

    return FALSE;
  }

  m_harySocket[nIndex] = hSocket;

  return TRUE;
}


BOOL CDasBootServerApp::SetEvent(int nIndex, WSAEVENT hEvent)
{
  if (nIndex < 0 || nIndex >= WSA_MAXIMUM_WAIT_EVENTS)
  {
    TRACE(_T("添加event至event数组时，超出数组的范围（0-63）"));

    return FALSE;
  }

  m_haryEvent[nIndex] = hEvent;

  return 0;
}


BOOL CDasBootServerApp::AddSocketEvent(SOCKET hSocket, WSAEVENT hEvent)
{
  if (m_nSocketEventNum >= WSA_MAXIMUM_WAIT_EVENTS)
  {
    TRACE(_T("无法添加socketevent，当前数量已达64上限"));

    return FALSE;
  }

  SetSocket(m_nSocketEventNum, hSocket);
  SetEvent(m_nSocketEventNum, hEvent);

  IncreaseSocketEventNum();

  return TRUE;
}


BOOL CDasBootServerApp::DestorySocketEvent(int nIndex)
{
  if (nIndex < 0 || nIndex >= WSA_MAXIMUM_WAIT_EVENTS)
  {
    TRACE(_T("无法删除socketevent，超出数组的范围（0-63）"));

    return FALSE;
  }

  if (m_haryEvent[nIndex] == 0)
  {
    TRACE(_T("无法删除socketevent，目标slot并不存在socketevent"));

    return FALSE;
  }

  closesocket(m_harySocket[nIndex]);
  WSACloseEvent(m_haryEvent[nIndex]);

  for (int i = nIndex; i < WSA_MAXIMUM_WAIT_EVENTS - 1; i++)
  {
    m_harySocket[nIndex] = m_harySocket[nIndex + 1];
    m_haryEvent[nIndex] = m_haryEvent[nIndex + 1];
  }

  DecreaseSocketEventNum();

  return 0;
}


WSAEVENT FAR * CDasBootServerApp::GetEventAry()
{
  return m_haryEvent;
}


SOCKET CDasBootServerApp::GetSocketFromSocketevent(int nIndex)
{
  return m_harySocket[nIndex];
}


WSAEVENT CDasBootServerApp::GetEventFromSocketevent(int nIndex)
{
  return m_haryEvent[nIndex];
}
