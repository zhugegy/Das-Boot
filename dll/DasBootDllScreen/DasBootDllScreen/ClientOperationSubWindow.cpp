// ClientOperationSubWindow.cpp : implementation file
//

#include "stdafx.h"
#include "DasBootDllScreen.h"
#include "ClientOperationSubWindow.h"
#include "afxdialogex.h"


// CClientOperationSubWindow dialog

IMPLEMENT_DYNAMIC(CClientOperationSubWindow, CDialogEx)

CClientOperationSubWindow::CClientOperationSubWindow(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DLL_SCREEN, pParent)
{

}

CClientOperationSubWindow::~CClientOperationSubWindow()
{
}

void CClientOperationSubWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CClientOperationSubWindow, CDialogEx)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_SCREEN_START, &CClientOperationSubWindow::OnBnClickedButtonClientOperationSubWindowScreenStart)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_SCREEN_STOP, &CClientOperationSubWindow::OnBnClickedButtonClientOperationSubWindowScreenStop)
END_MESSAGE_MAP()

// CClientOperationSubWindow message handlers

void CClientOperationSubWindow::OnBnClickedButtonClientOperationSubWindowScreenStart()
{
  // TODO: Add your control notification handler code here
  m_bIsMonitoring = true;
  this->m_pfnSMO(this->m_hSocket, _T("GetScreenSize0C"), _T("nothing"), -1);
}

BOOL CClientOperationSubWindow::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // TODO:  Add extra initialization here
  m_bIsMonitoring = false;

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CClientOperationSubWindow::OnBnClickedButtonClientOperationSubWindowScreenStop()
{
  // TODO: Add your control notification handler code here
  m_bIsMonitoring = false;
}
