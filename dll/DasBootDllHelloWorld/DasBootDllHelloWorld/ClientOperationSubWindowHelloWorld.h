#pragma once
#include "DllCommon.h"

// CClientOperationSubWindowHelloWorld dialog

class CClientOperationSubWindowHelloWorld : public CDialogEx
{
	DECLARE_DYNAMIC(CClientOperationSubWindowHelloWorld)

public:
	CClientOperationSubWindowHelloWorld(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClientOperationSubWindowHelloWorld();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DLL_HELLO_WORLD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedButtonClientOperationSubWindowHelloWorldSend();

public:
  SOCKET m_hSocket;
  pfnSendMessageOut m_pfnSMO;
  CString m_strEdit;
  virtual BOOL OnInitDialog();
};
