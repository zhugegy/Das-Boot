// ClientOperationMainWindow.cpp : implementation file
//

#include "stdafx.h"
#include "DasBootServer.h"
#include "ClientOperationMainWindow.h"
#include "afxdialogex.h"
#include "DasBootBasicCommands.h"

// CClientOperationMainWindow dialog

IMPLEMENT_DYNAMIC(CClientOperationMainWindow, CDialogEx)


CClientOperationMainWindow::CClientOperationMainWindow(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CLIENT_OPERATION_MAIN_WINDOW, pParent)
  , m_editModuleIntroduction(_T(""))
{

}

CClientOperationMainWindow::~CClientOperationMainWindow()
{
}

void CClientOperationMainWindow::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COMBO_CLIENT_OPERATION_MAIN_WINDOW_CHOOSE_MODULE, m_cbbModuleName);
  DDX_Text(pDX, IDC_EDIT_CLIENT_OPERATION_MAIN_WINDOW_MODULE_INTRODUCTION, m_editModuleIntroduction);
  DDX_Control(pDX, IDC_LIST_CLIENT_OPEATION_MAIN_WINDOW_MODULE_STATE, m_lstctlModuleStateInClient);
}


BEGIN_MESSAGE_MAP(CClientOperationMainWindow, CDialogEx)

  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_MAIN_WINDOW_CONFIRM, &CClientOperationMainWindow::OnBnClickedButtonClientOperationMainWindowConfirm)
  ON_CBN_SELCHANGE(IDC_COMBO_CLIENT_OPERATION_MAIN_WINDOW_CHOOSE_MODULE, &CClientOperationMainWindow::OnSelchangeComboClientOperationMainWindowChooseModule)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_MAIN_WINDOW_RE_RECON, &CClientOperationMainWindow::OnBnClickedButtonClientOperationMainWindowReRecon)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_MAIN_WINDOW_SEND_MODULE, &CClientOperationMainWindow::OnBnClickedButtonClientOperationMainWindowSendModule)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_MAIN_WINDOW_LOAD_MODULE, &CClientOperationMainWindow::OnBnClickedButtonClientOperationMainWindowLoadModule)
END_MESSAGE_MAP()


// CClientOperationMainWindow message handlers



BOOL CClientOperationMainWindow::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // TODO:  Add extra initialization here
  m_hSocket = 0;

  m_cbbModuleName.AddString(_T("选择想使用的功能模块"));
  m_cbbModuleName.SetCurSel(0);

  for (POSITION pos = theApp.m_lstCurrentDllNames.GetHeadPosition(); pos != NULL; )
  {
    m_cbbModuleName.InsertString(1, theApp.m_lstCurrentDllNames.GetNext(pos));
  }

  m_lstctlModuleStateInClient.ModifyStyle(0, LVS_REPORT);
  DWORD dwStye = m_lstctlModuleStateInClient.GetExtendedStyle();
  dwStye = dwStye | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
  m_lstctlModuleStateInClient.SetExtendedStyle(dwStye);

  m_lstctlModuleStateInClient.InsertColumn(0, _T("侦测项目"), 0, 150);
  m_lstctlModuleStateInClient.InsertColumn(1, _T("侦测结果"), 0, 70);

  m_lstctlModuleStateInClient.InsertItem(LVIF_TEXT | LVIF_STATE, 0, 
    _T("模块文件是否已存在"), 0, 0, 0, 0);

  m_lstctlModuleStateInClient.SetItemText(0, 1, _T("未知"));

  m_lstctlModuleStateInClient.InsertItem(LVIF_TEXT | LVIF_STATE, 1, 
    _T("模块文件是否已被加载"), 0, 0, 0, 0);

  m_lstctlModuleStateInClient.SetItemText(1, 1, _T("未知"));

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CClientOperationMainWindow::OnBnClickedButtonClientOperationMainWindowConfirm()
{
  // TODO: Add your control notification handler code here
  int n = m_cbbModuleName.GetCurSel();
  if (m_cbbModuleName.GetCurSel() != 0)
  {
    CString strDllName;
    m_cbbModuleName.GetLBText(m_cbbModuleName.GetCurSel(), strDllName);

    pfnDBExportServerOperationConfirm pfnConfirm;
    pfnConfirm = theApp.m_mapConfirmFunctions[strDllName.GetBuffer(0)];

    CClientContext *pClientContext = NULL;
    theApp.m_objClientManager.FindAt(this->m_hSocket, pClientContext);

    pfnConfirm(this->m_hSocket, this, pClientContext, &SendMessageOut);
  }
}


void CClientOperationMainWindow::OnSelchangeComboClientOperationMainWindowChooseModule()
{
  // TODO: Add your control notification handler code here
  int n = m_cbbModuleName.GetCurSel();
  if (m_cbbModuleName.GetCurSel() != 0)
  {
    CString strDllName;
    m_cbbModuleName.GetLBText(m_cbbModuleName.GetCurSel(), strDllName);

    CString strDllIntroduction;

    std::string strTmp= theApp.m_mapDllIntroduction[strDllName.GetBuffer(0)];
    strDllIntroduction.Format("%s", strTmp.c_str());
    
    m_editModuleIntroduction = strDllIntroduction;
  }

  m_lstctlModuleStateInClient.SetItemText(0, 1, _T("未知"));
  m_lstctlModuleStateInClient.SetItemText(1, 1, _T("未知"));

  UpdateData(FALSE);
}


void CClientOperationMainWindow::OnBnClickedButtonClientOperationMainWindowReRecon()
{
  // TODO: Add your control notification handler code here
  int n = m_cbbModuleName.GetCurSel();
  if (m_cbbModuleName.GetCurSel() != 0)
  {
    CString strDllName;
    m_cbbModuleName.GetLBText(m_cbbModuleName.GetCurSel(), strDllName);

    DBBCReconModuleStatusC(m_hSocket, strDllName.GetBuffer(0));
  }

}


void CClientOperationMainWindow::OnBnClickedButtonClientOperationMainWindowSendModule()
{
  // TODO: Add your control notification handler code here
  int n = m_cbbModuleName.GetCurSel();
  if (m_cbbModuleName.GetCurSel() != 0)
  {
    CString strDllName;
    m_cbbModuleName.GetLBText(m_cbbModuleName.GetCurSel(), strDllName);

    FILE *pFile = fopen(strDllName.GetBuffer(0), "rb");
    fseek(pFile, 0, SEEK_END);
    int nSize = ftell(pFile);
    fclose(pFile);

    CClientContext *pClientContext = NULL;
    theApp.m_objClientManager.FindAt(m_hSocket, pClientContext);

    pClientContext->m_pFileTransferWindow->m_prgctlFileTransfer.SetRange(0, 
      nSize / 1024);

    pClientContext->m_pFileTransferWindow->m_prgctlFileTransfer.SetPos(0);

    pClientContext->m_pFileTransferWindow->ShowWindow(SW_SHOW);

    DBBCGiveClientFileC(m_hSocket, strDllName.GetBuffer(0), 
    strDllName.GetBuffer(0));
  }

}


void CClientOperationMainWindow::OnBnClickedButtonClientOperationMainWindowLoadModule()
{
  // TODO: Add your control notification handler code here
  int n = m_cbbModuleName.GetCurSel();
  if (m_cbbModuleName.GetCurSel() != 0)
  {
    CString strDllName;
    m_cbbModuleName.GetLBText(m_cbbModuleName.GetCurSel(), strDllName);

    DBBCLoadClientModuleC(m_hSocket, strDllName.GetBuffer(0));
  }

}
