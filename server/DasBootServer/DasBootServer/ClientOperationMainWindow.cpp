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
}


BEGIN_MESSAGE_MAP(CClientOperationMainWindow, CDialogEx)

  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_MAIN_WINDOW_CONFIRM, &CClientOperationMainWindow::OnBnClickedButtonClientOperationMainWindowConfirm)
  ON_CBN_SELCHANGE(IDC_COMBO_CLIENT_OPERATION_MAIN_WINDOW_CHOOSE_MODULE, &CClientOperationMainWindow::OnSelchangeComboClientOperationMainWindowChooseModule)
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

    UpdateData(FALSE);
  }
}
