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
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_INIT, &CClientOperationSubWindow::OnBnClickedButtonClientOperationSubWindowInit)
END_MESSAGE_MAP()


// CClientOperationSubWindow message handlers


void CClientOperationSubWindow::OnBnClickedButtonClientOperationSubWindowScreenStart()
{
  // TODO: Add your control notification handler code here
  this->m_pfnSMO(this->m_hSocket, _T("CaptureScreen0C"), _T("nothing"), -1);

}

void CClientOperationSubWindow::OnBnClickedButtonClientOperationSubWindowInit()
{
  // TODO: Add your control notification handler code here
  this->m_pfnSMO(this->m_hSocket, _T("GetScreenSize0C"), _T("nothing"), -1);
}

