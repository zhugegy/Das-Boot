#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CClientOperationMainWindow dialog

class CClientOperationMainWindow : public CDialogEx
{
	DECLARE_DYNAMIC(CClientOperationMainWindow)

public:
	CClientOperationMainWindow(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClientOperationMainWindow();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CLIENT_OPERATION_MAIN_WINDOW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

  SOCKET m_hSocket;
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedButtonClientOperationMainWindowConfirm();
  afx_msg void OnSelchangeComboClientOperationMainWindowChooseModule();
  CComboBox m_cbbModuleName;
  CString m_editModuleIntroduction;
  CListCtrl m_lstctlModuleStateInClient;
  afx_msg void OnBnClickedButtonClientOperationMainWindowReRecon();
  afx_msg void OnBnClickedButtonClientOperationMainWindowSendModule();
  afx_msg void OnBnClickedButtonClientOperationMainWindowLoadModule();
};
