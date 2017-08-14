#pragma once

#include "DllCommon.h"

// CClientOperationSubWindowRemoteCmd dialog

class CClientOperationSubWindowRemoteCmd : public CDialogEx
{
	DECLARE_DYNAMIC(CClientOperationSubWindowRemoteCmd)

public:
	CClientOperationSubWindowRemoteCmd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClientOperationSubWindowRemoteCmd();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DLL_REMOTE_CMD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  CString m_edtctlStrConsoleContent;
  CString m_edtctlStrConsoleCommand;

public:
  SOCKET m_hSocket;
  pfnSendMessageOut m_pfnSMO;
  afx_msg void OnBnClickedButtonClientOperationSubWindowSendCommand();
  virtual BOOL OnInitDialog();
  afx_msg void OnClose();
  afx_msg void OnBnClickedButtonClientOperationSubWindowInitRemoteCmd();
};
