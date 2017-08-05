#pragma once
#include "DllCommon.h"
#include "afxcmn.h"


// CClientOperationSubWindowProcessAndThread dialog

class CClientOperationSubWindowProcessAndThread : public CDialogEx
{
	DECLARE_DYNAMIC(CClientOperationSubWindowProcessAndThread)

public:
	CClientOperationSubWindowProcessAndThread(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClientOperationSubWindowProcessAndThread();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DLL_PROCESS_AND_THREAD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  SOCKET m_hSocket;
  pfnSendMessageOut m_pfnSMO;

  virtual BOOL OnInitDialog();
  CListCtrl m_lstctlProcessList;
  afx_msg void OnBnClickedButtonClientOperationSubWindowRefreshProcess();
  afx_msg void OnBnClickedButtonClientOperationSubWindowTerminateProcess();
};
