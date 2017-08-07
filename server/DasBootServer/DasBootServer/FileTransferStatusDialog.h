#pragma once
#include "afxcmn.h"


// CFileTransferStatusDialog dialog

class CFileTransferStatusDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CFileTransferStatusDialog)

public:
	CFileTransferStatusDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFileTransferStatusDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CLIENT_OPERATION_MAIN_WINDOW_FILE_TRANSFER_SUB_WINDOW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  int m_nFileSizeKB;
  CProgressCtrl m_prgctlFileTransfer;
};
