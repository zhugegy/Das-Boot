// FileTransferStatusDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DasBootServer.h"
#include "FileTransferStatusDialog.h"
#include "afxdialogex.h"


// CFileTransferStatusDialog dialog

IMPLEMENT_DYNAMIC(CFileTransferStatusDialog, CDialogEx)

CFileTransferStatusDialog::CFileTransferStatusDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CLIENT_OPERATION_MAIN_WINDOW_FILE_TRANSFER_SUB_WINDOW, pParent)
{

}

CFileTransferStatusDialog::~CFileTransferStatusDialog()
{
}

void CFileTransferStatusDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_PROGRESS1, m_prgctlFileTransfer);
}


BEGIN_MESSAGE_MAP(CFileTransferStatusDialog, CDialogEx)
END_MESSAGE_MAP()


// CFileTransferStatusDialog message handlers
