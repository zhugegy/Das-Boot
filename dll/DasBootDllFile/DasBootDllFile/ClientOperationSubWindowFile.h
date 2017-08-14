#pragma once
#include "DllCommon.h"
#include "afxcmn.h"
#include "FileTree.h"

// CClientOperationSubWindowFile dialog

class CClientOperationSubWindowFile : public CDialogEx
{
	DECLARE_DYNAMIC(CClientOperationSubWindowFile)

public:
	CClientOperationSubWindowFile(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClientOperationSubWindowFile();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DLL_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  SOCKET m_hSocket;
  pfnSendMessageOut m_pfnSMO;
  afx_msg void OnBnClickedButtonClientOperationSubWindowFileStart();
  CFileTree m_Tree;
  afx_msg void OnBnClickedButtonClientOperationSubWindowFileSaveSingleFile();
  afx_msg void OnBnClickedButtonClientOperationSubWindowFileSaveMultFiles();
};
