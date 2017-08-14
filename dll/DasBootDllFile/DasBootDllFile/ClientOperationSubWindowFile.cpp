// ClientOperationSubWindowFile.cpp : implementation file
//

#include "stdafx.h"
#include "DasBootDllFile.h"
#include "ClientOperationSubWindowFile.h"
#include "afxdialogex.h"


// CClientOperationSubWindowFile dialog

IMPLEMENT_DYNAMIC(CClientOperationSubWindowFile, CDialogEx)

CClientOperationSubWindowFile::CClientOperationSubWindowFile(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DLL_FILE, pParent)
{

}

CClientOperationSubWindowFile::~CClientOperationSubWindowFile()
{
}

void CClientOperationSubWindowFile::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_TREE_CLIENT_OPERATION_SUB_WINDOW_FILE_TREE, m_Tree);
}


BEGIN_MESSAGE_MAP(CClientOperationSubWindowFile, CDialogEx)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_FILE_START, &CClientOperationSubWindowFile::OnBnClickedButtonClientOperationSubWindowFileStart)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_FILE_SAVE_SINGLE_FILE, &CClientOperationSubWindowFile::OnBnClickedButtonClientOperationSubWindowFileSaveSingleFile)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_FILE_SAVE_MULT_FILES, &CClientOperationSubWindowFile::OnBnClickedButtonClientOperationSubWindowFileSaveMultFiles)
END_MESSAGE_MAP()


// CClientOperationSubWindowFile message handlers


void CClientOperationSubWindowFile::OnBnClickedButtonClientOperationSubWindowFileStart()
{
  // TODO: Add your control notification handler code here
  m_pfnSMO(m_hSocket, _T("FileViewStart0C"), _T("nothing"), -1);
  AfxMessageBox(_T("时间有限，功能没有做出来。这其实是本地的文件，并不是客户机文件。"));

}


void CClientOperationSubWindowFile::OnBnClickedButtonClientOperationSubWindowFileSaveSingleFile()
{
  // TODO: Add your control notification handler code here
  CFileDialog dlg(FALSE);

  dlg.m_ofn.lpstrTitle = _T("保存单个客户端文件到本地");

  if (IDOK == dlg.DoModal())
  {
    AfxMessageBox(_T("时间有限，功能没有做出来。以后再做。"));
  }
}


void CClientOperationSubWindowFile::OnBnClickedButtonClientOperationSubWindowFileSaveMultFiles()
{
  // TODO: Add your control notification handler code here
  AfxMessageBox(_T("时间有限，功能没有做出来。以后再做。"));
}
