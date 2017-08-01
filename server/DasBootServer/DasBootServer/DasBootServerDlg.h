
// DasBootServerDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CDasBootServerDlg dialog
class CDasBootServerDlg : public CDialogEx
{
public:
#define WM_PKTHANDLEMSG WM_USER + 1
// Construction
public:
	CDasBootServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DASBOOTSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
  afx_msg HRESULT OnPktHandleMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButtonMainStartListening();
  afx_msg void OnBnClickedButtonMainStopListening();
  CListCtrl m_lstctlClientList;
};
