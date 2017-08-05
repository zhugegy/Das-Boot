// ClientOperationSubWindowProcessAndThread.cpp : implementation file
//

#include "stdafx.h"
#include "DasBootDllProcessAndThread.h"
#include "ClientOperationSubWindowProcessAndThread.h"
#include "afxdialogex.h"


// CClientOperationSubWindowProcessAndThread dialog

IMPLEMENT_DYNAMIC(CClientOperationSubWindowProcessAndThread, CDialogEx)

CClientOperationSubWindowProcessAndThread::CClientOperationSubWindowProcessAndThread(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DLL_PROCESS_AND_THREAD, pParent)
{

}

CClientOperationSubWindowProcessAndThread::~CClientOperationSubWindowProcessAndThread()
{
}

void CClientOperationSubWindowProcessAndThread::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_CLIENT_OPERATION_SUB_WINDOW_PROCESS_LIST, m_lstctlProcessList);
}


BEGIN_MESSAGE_MAP(CClientOperationSubWindowProcessAndThread, CDialogEx)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_REFRESH_PROCESS, &CClientOperationSubWindowProcessAndThread::OnBnClickedButtonClientOperationSubWindowRefreshProcess)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_TERMINATE_PROCESS, &CClientOperationSubWindowProcessAndThread::OnBnClickedButtonClientOperationSubWindowTerminateProcess)
END_MESSAGE_MAP()


// CClientOperationSubWindowProcessAndThread message handlers


BOOL CClientOperationSubWindowProcessAndThread::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // TODO:  Add extra initialization here
  m_lstctlProcessList.ModifyStyle(0, LVS_REPORT);
  DWORD dwStye = m_lstctlProcessList.GetExtendedStyle();
  dwStye = dwStye | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
  m_lstctlProcessList.SetExtendedStyle(dwStye);

  m_lstctlProcessList.InsertColumn(0, _T("进程名称"), 0, 128);
  m_lstctlProcessList.InsertColumn(1, _T("PID"), 0, 100);
  m_lstctlProcessList.InsertColumn(2, _T("线程数量"), 0, 70);
  m_lstctlProcessList.InsertColumn(3, _T("父进程PID"), 0, 100);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}



void CClientOperationSubWindowProcessAndThread::OnBnClickedButtonClientOperationSubWindowRefreshProcess()
{
  // TODO: Add your control notification handler code here
  this->m_pfnSMO(this->m_hSocket, _T("RefreshProcessC"), 
    _T("nothing(list all)"), -1);

}


void CClientOperationSubWindowProcessAndThread::OnBnClickedButtonClientOperationSubWindowTerminateProcess()
{
  // TODO: Add your control notification handler code here
  CString strPid;  //用CString不用考虑缓冲区大小问题

  for (int i = 0; i < m_lstctlProcessList.GetItemCount(); i++)
  {
    if (m_lstctlProcessList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
    {
      int nPid = m_lstctlProcessList.GetItemData(i);

      strPid.Format("%d", nPid);

      this->m_pfnSMO(this->m_hSocket, _T("TerminateProceC"), 
        strPid.GetBuffer(0), -1);
    }
  }
}
