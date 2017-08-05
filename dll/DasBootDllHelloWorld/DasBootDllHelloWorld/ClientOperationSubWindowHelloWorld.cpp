// ClientOperationSubWindowHelloWorld.cpp : implementation file
//

#include "stdafx.h"
#include "DasBootDllHelloWorld.h"
#include "ClientOperationSubWindowHelloWorld.h"
#include "afxdialogex.h"

#include "DllCommon.h"


// CClientOperationSubWindowHelloWorld dialog

IMPLEMENT_DYNAMIC(CClientOperationSubWindowHelloWorld, CDialogEx)

CClientOperationSubWindowHelloWorld::CClientOperationSubWindowHelloWorld(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DLL_HELLO_WORLD, pParent)
  , m_strEdit(_T(""))
{

}

CClientOperationSubWindowHelloWorld::~CClientOperationSubWindowHelloWorld()
{
}

void CClientOperationSubWindowHelloWorld::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT1, m_strEdit);
}


BEGIN_MESSAGE_MAP(CClientOperationSubWindowHelloWorld, CDialogEx)
  ON_BN_CLICKED(IDOK, &CClientOperationSubWindowHelloWorld::OnBnClickedOk)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_HELLO_WORLD_SEND, &CClientOperationSubWindowHelloWorld::OnBnClickedButtonClientOperationSubWindowHelloWorldSend)
END_MESSAGE_MAP()


// CClientOperationSubWindowHelloWorld message handlers


void CClientOperationSubWindowHelloWorld::OnBnClickedOk()
{
  // TODO: Add your control notification handler code here


  CDialogEx::OnOK();
}


void CClientOperationSubWindowHelloWorld::OnBnClickedButtonClientOperationSubWindowHelloWorldSend()
{
  // TODO: Add your control notification handler code here
  this->m_pfnSMO(this->m_hSocket, _T("HelloWorld0000C"), _T("DLL"), -1);
}


BOOL CClientOperationSubWindowHelloWorld::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // TODO:  Add extra initialization here
  m_strEdit = _T("初始界面，请按按钮发送命令");

  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}
