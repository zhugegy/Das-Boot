// ClientOperationSubWindowRemoteCmd.cpp : implementation file
//

#include "stdafx.h"
#include "DasBootDllRemoteCmd.h"
#include "ClientOperationSubWindowRemoteCmd.h"
#include "afxdialogex.h"


// CClientOperationSubWindowRemoteCmd dialog

IMPLEMENT_DYNAMIC(CClientOperationSubWindowRemoteCmd, CDialogEx)

CClientOperationSubWindowRemoteCmd::CClientOperationSubWindowRemoteCmd(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DLL_REMOTE_CMD, pParent)
  , m_edtctlStrConsoleContent(_T(""))
  , m_edtctlStrConsoleCommand(_T(""))
{

}

CClientOperationSubWindowRemoteCmd::~CClientOperationSubWindowRemoteCmd()
{
}

void CClientOperationSubWindowRemoteCmd::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_CLIENT_OPERATION_SUB_WINDOW_CONSOLE_CONTENT, m_edtctlStrConsoleContent);
  DDX_Text(pDX, IDC_EDIT_CLIENT_OPERATION_SUB_WINDOW_CONSOLE_COMMAND_INPUT, m_edtctlStrConsoleCommand);
}


BEGIN_MESSAGE_MAP(CClientOperationSubWindowRemoteCmd, CDialogEx)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_SEND_COMMAND, &CClientOperationSubWindowRemoteCmd::OnBnClickedButtonClientOperationSubWindowSendCommand)
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_INIT_REMOTE_CMD, &CClientOperationSubWindowRemoteCmd::OnBnClickedButtonClientOperationSubWindowInitRemoteCmd)
END_MESSAGE_MAP()


// CClientOperationSubWindowRemoteCmd message handlers


void CClientOperationSubWindowRemoteCmd::OnBnClickedButtonClientOperationSubWindowSendCommand()
{
  // TODO: Add your control notification handler code here
  UpdateData(TRUE);

  if (m_edtctlStrConsoleCommand.GetLength() != 0)
  {
    m_pfnSMO(m_hSocket, _T("RmotCmdSendCmdC"),
      m_edtctlStrConsoleCommand.GetBuffer(0), -1);
  }

  m_edtctlStrConsoleCommand.Format("%s", _T(""));

  UpdateData(FALSE);
}


BOOL CClientOperationSubWindowRemoteCmd::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // TODO:  Add extra initialization here
  CWnd *pWndTmp = GetDlgItem(
    IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_SEND_COMMAND);
  pWndTmp->EnableWindow(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CClientOperationSubWindowRemoteCmd::OnClose()
{
  // TODO: Add your message handler code here and/or call default
  m_pfnSMO(m_hSocket, _T("CloseRemoteCmdC"), _T("nothing"), -1);

  CDialogEx::OnClose();
}


void CClientOperationSubWindowRemoteCmd::OnBnClickedButtonClientOperationSubWindowInitRemoteCmd()
{
  // TODO: Add your control notification handler code here
  m_pfnSMO(m_hSocket, _T("StartRemoteCmdC"), _T("nothing"), -1);
}
