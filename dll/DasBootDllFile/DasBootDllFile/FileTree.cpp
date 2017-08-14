#include "stdafx.h"

#include "FileTree.h"
#include "resource.h"


CFileTree::CFileTree()
{
  m_clText = RGB(58, 79, 107);
  m_clBk = RGB(234, 242, 255);
  m_clLine = RGB(58, 79, 107);

  m_nFlags = 0;

}

CFileTree::~CFileTree()
{

}


BEGIN_MESSAGE_MAP(CFileTree, CTreeCtrl)
  //{{AFX_MSG_MAP(CFileTree)
  ON_WM_LBUTTONDOWN()
  //}}AFX_MSG_MAP
  ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemExpanded)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileTree message handlers

void CFileTree::PreSubclassWindow()
{
  CTreeCtrl::PreSubclassWindow();
  m_StateImages.Create(12, 14, ILC_COLOR24 | ILC_MASK, 3, 1);

  CBitmap bmp;
  bmp.LoadBitmap(IDB_NOMAL1);
  m_StateImages.Add(&bmp, RGB(255, 255, 255));
  bmp.DeleteObject();

  bmp.LoadBitmap(IDB_NOMAL2);
  m_StateImages.Add(&bmp, RGB(255, 255, 255));
  bmp.DeleteObject();

  bmp.LoadBitmap(IDB_CHECKED);
  m_StateImages.Add(&bmp, RGB(255, 255, 255));
  bmp.DeleteObject();

  bmp.LoadBitmap(IDB_FULLCHECKED);
  m_StateImages.Add(&bmp, RGB(255, 255, 255));
  bmp.DeleteObject();

  //设置文本颜色
  SetTextColor(m_clText);
  //设置背景颜色
  SetBkColor(m_clBk);
  //设置线条颜色
  //SendMessage(TV_FIRST + 40, 0, (LPARAM)m_clLine);
  SetLineColor(m_clLine);
  LoadDiskName();
}

BOOL CFileTree::SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask, BOOL bSearch)
{
  BOOL bRet = CTreeCtrl::SetItemState(hItem, nState, nStateMask);
  UINT nStatus = nState >> 12;
  if (nStatus != 0)
  {
    if (bSearch)
      RansackChild(hItem, nStatus);
    RansackParentAndChild(hItem, nStatus);
  }
  return bRet;
}

void CFileTree::RansackChild(HTREEITEM hItem, UINT nState)
{
  HTREEITEM hChildItem, hBrotherItem;
  //查找子节点
  hChildItem = GetChildItem(hItem);
  if (hChildItem != NULL)
  {
    //将所有子节点的状态设置与父节点相同
    CTreeCtrl::SetItemState(hChildItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
    //再递归处理子节点的子节点
    RansackChild(hChildItem, nState);
    //搜索子节点的兄弟节点
    hBrotherItem = GetNextSiblingItem(hChildItem);
    while (hBrotherItem)
    {
      //设置子节点的兄弟节点状态与当前节点的状态一致
      int nStatus = GetItemState(hBrotherItem, TVIS_STATEIMAGEMASK) >> 12;
      if (nStatus != 0)
      {
        CTreeCtrl::SetItemState(hBrotherItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
      }
      //再递归处理兄弟节点
      RansackChild(hBrotherItem, nState);
      hBrotherItem = GetNextSiblingItem(hBrotherItem);
    }
  }
}

void CFileTree::RansackParentAndChild(HTREEITEM hItem, UINT nState)
{
  HTREEITEM hNextItem, hPrevItem, hParentItem;
  //查找父节点，没有就结束
  hParentItem = GetParentItem(hItem);
  if (hParentItem != NULL)
  {
    UINT nStatus = nState;	//设初始值，防止没有兄弟节点时出错
                            //查找当前节点的所有兄弟节点,如果所有兄弟节点状态都相同,
                            //设置父节点的状态
                            //查找当前节点下面的兄弟节点的状态
    hNextItem = GetNextSiblingItem(hItem);
    while (hNextItem != NULL)
    {
      nStatus = GetItemState(hNextItem, TVIS_STATEIMAGEMASK) >> 12;
      if (nStatus != nState && nStatus != 0)
        break;
      else
        hNextItem = GetNextSiblingItem(hNextItem);
    }
    if (nStatus == nState)
    {
      //查找当前节点上面的兄弟节点的状态
      hPrevItem = GetPrevSiblingItem(hItem);
      while (hPrevItem != NULL)
      {
        nStatus = GetItemState(hPrevItem, TVIS_STATEIMAGEMASK) >> 12;
        if (nStatus != nState && nStatus != 0)
          break;
        else
          hPrevItem = GetPrevSiblingItem(hPrevItem);
      }
    }
    if (nStatus == nState || nStatus == 0)
    {
      nStatus = GetItemState(hParentItem, TVIS_STATEIMAGEMASK) >> 12;
      if (nStatus != 0)
      {
        //如果状态一致，则父节点的状态与当前节点的状态一致
        CTreeCtrl::SetItemState(hParentItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
      }
      //再递归处理父节点的兄弟节点和其父节点
      RansackParentAndChild(hParentItem, nState);
    }
    else
    {
      //状态不一致，则当前节点的父节点、父节点的父节点……状态均为第三态
      hParentItem = GetParentItem(hItem);
      while (hParentItem != NULL)
      {
        nStatus = GetItemState(hParentItem, TVIS_STATEIMAGEMASK) >> 12;
        if (nStatus != 0)
        {
          CTreeCtrl::SetItemState(hParentItem, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK);
        }
        hParentItem = GetParentItem(hParentItem);
      }
    }
  }
}

void CFileTree::OnLButtonDown(UINT nFlags, CPoint point)
{
  HTREEITEM hItemInfo = HitTest(point, &m_nFlags);
  nFlags = m_nFlags;
  //TVHT_ONITEMSTATEICON表示用户定义的视图项的图标状态
  if (m_nFlags & TVHT_ONITEMSTATEICON)
  {
    //State: 0无状态 1没有选择 2部分选择 3全部选择
    //12到15位表示视图项的图像状态索引
    UINT nState = GetItemState(hItemInfo, TVIS_STATEIMAGEMASK) >> 12;
    nState = (nState == 3) ? 1 : 3;
    SetItemState(hItemInfo, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
  }
  else
    CTreeCtrl::OnLButtonDown(nFlags, point);
}

BOOL CFileTree::LoadSysFileIcon()
{
  SHFILEINFO shInfo;							//定义文件信息
  memset(&shInfo, 0, sizeof(SHFILEINFO));		//初始化文件信息
  m_hImage = (HIMAGELIST)SHGetFileInfo("c:\\", 0, &shInfo, sizeof(SHFILEINFO),
    SHGFI_SYSICONINDEX | SHGFI_SMALLICON);	//获取系统文件图标
  if (m_hImage != NULL)
  {
    BOOL bRet = m_ImageList.Attach(m_hImage);	//关联图像列表句柄 
    if (bRet)
    {
      HICON hIcon = NULL;
      hIcon = ExtractIcon(NULL, "explorer.exe", 0);
      //添加我的电脑图标
      int nIcon = m_ImageList.Add(hIcon);
      m_ImageList.SetBkColor(m_clBk);
      SetImageList(&m_ImageList, TVSIL_NORMAL);
      //设置状态图像列表
      SetImageList(&m_StateImages, TVSIL_STATE);
    }
    return	bRet;
  }
  return FALSE;
}

void CFileTree::LoadDiskName()
{
  if (LoadSysFileIcon() == TRUE)	//首先加载系统文件图像列表
  {
    //添加根节点
    HTREEITEM hRoot = AddItem(TVI_ROOT, "我的电脑");
    char  pchDrives[128] = { 0 };
    char* pchDrive;
    GetLogicalDriveStrings(sizeof(pchDrives), pchDrives);
    pchDrive = pchDrives;
    while (*pchDrive)
    {
      HTREEITEM hParent = AddItem(hRoot, pchDrive);
      if (IsSubDir(pchDrive))
        InsertItem("", 0, 0, hParent);
      pchDrive += strlen(pchDrive) + 1;
    }
  }
}


void CFileTree::ShowPath(HTREEITEM hParent, LPCTSTR lpszPath)
{
  CFileFind flFind;
  CString   csPath = lpszPath;
  BOOL      bFind;
  CSortString strDirArray;
  CSortString strFileArray;

  if (csPath.Right(1) != "\\")
    csPath += "\\";
  csPath += "*.*";

  bFind = flFind.FindFile(csPath);
  while (bFind)
  {
    bFind = flFind.FindNextFile();
    if (flFind.IsDirectory() && !flFind.IsDots())
    {
      strDirArray.Add(flFind.GetFilePath());
    }
    if (!flFind.IsDirectory())
      strFileArray.Add(flFind.GetFilePath());
  }
  strDirArray.Sort();
  SetRedraw(FALSE);
  CWaitCursor wait;
  for (int i = 0; i < strDirArray.GetSize(); i++)
  {
    HTREEITEM hItem = AddItem(hParent, strDirArray.GetAt(i));
    UINT nState = GetItemState(hParent, TVIS_STATEIMAGEMASK) >> 12;
    SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
    if (IsSubDir(strDirArray.GetAt(i)))
    {
      InsertItem("", 0, 0, hItem);
    }
  }
  strFileArray.Sort();
  for (int i = 0; i < strFileArray.GetSize(); i++)
  {
    HTREEITEM hItem = AddItem(hParent, strFileArray.GetAt(i));
    UINT nState = GetItemState(hParent, TVIS_STATEIMAGEMASK) >> 12;
    SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
  }
  SetRedraw(TRUE);
}

CString CFileTree::GetFullPath(HTREEITEM hItem)
{
  CString csRet;	//记录目录
  CString csCurDir;
  HTREEITEM hParent = hItem;
  csRet = "";
  //遍历父节点
  HTREEITEM hRoot = GetRootItem();
  while (hParent != hRoot)
  {
    csCurDir = GetItemText(hParent);
    csCurDir += "\\";
    csRet = csCurDir + csRet;
    hParent = GetParentItem(hParent);
  }
  //除去结尾的"\"符号
  if (csRet.Right(1) == '\\')
    csRet.SetAt(csRet.GetLength() - 1, '\0');
  return csRet;
}

HTREEITEM CFileTree::AddItem(HTREEITEM hParent, LPCTSTR lpszPath)
{
  SHFILEINFO shFinfo;
  int nIcon, nIconSel;
  CString    csPath = lpszPath;
  if (csPath.Right(1) != '\\')
    csPath += "\\";
  if (hParent == TVI_ROOT)
  {
    nIcon = nIconSel = m_ImageList.GetImageCount() - 1;
  }
  else
  {
    //获取目录或文件的系统图标索引
    if (!SHGetFileInfo(csPath, 0, &shFinfo, sizeof(shFinfo), SHGFI_ICON | SHGFI_SMALLICON))
    {
      return NULL;
    }
    nIcon = shFinfo.iIcon;
    //获取目录或文件选中时的系统图标索引
    if (!SHGetFileInfo(csPath, 0, &shFinfo, sizeof(shFinfo),
      SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON))
    {
      return NULL;
    }
    nIconSel = shFinfo.iIcon;
  }
  if (csPath.Right(1) == "\\")
    csPath.SetAt(csPath.GetLength() - 1, '\0');
  return InsertItem(ExtractPath(csPath), nIcon, nIcon, hParent);
}

CString CFileTree::ExtractPath(LPCTSTR lpszPath)
{
  CString csPath = "";
  int nPos;
  csPath = lpszPath;
  //除去结尾的"\"
  if (csPath.Right(1) == '\\')
  {
    csPath.SetAt(csPath.GetLength() - 1, '\0');
  }
  nPos = csPath.ReverseFind('\\');
  if (nPos != -1)
    csPath = csPath.Mid(nPos + 1, csPath.GetLength());
  return (LPCTSTR)csPath;
}

BOOL CFileTree::IsSubDir(LPCTSTR lpszPath)
{
  CFileFind flFind;
  CString csPath = lpszPath;
  BOOL bFind = FALSE;
  //保证路径以\*.*结尾
  if (_tcslen(lpszPath) == 3)
  {
    if (lpszPath[1] == ':' && lpszPath[2] == '\\')
      csPath += "*.*";
    else
      csPath += "\\*.*";
  }
  else
  {
    csPath += "\\*.*";
  }
  bFind = flFind.FindFile(csPath);
  while (bFind)
  {
    bFind = flFind.FindNextFile();
    if (!flFind.IsDots())
    {
      return TRUE;
    }
  }
  return FALSE;
}
void CFileTree::OnItemExpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
  NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
  CString strPath;

  if (pNMTreeView->itemNew.state & TVIS_EXPANDED)
  {
    ExpandItem(pNMTreeView->itemNew.hItem, TVE_EXPAND);
  }
  else
  {
    if (pNMTreeView->itemNew.hItem != GetRootItem())
    {
      int nData = GetItemData(pNMTreeView->itemNew.hItem);
      if (nData != LOADDIR)
      {
        HTREEITEM hChild = GetChildItem(pNMTreeView->itemNew.hItem);
        while (hChild)
        {
          DeleteItem(hChild);
          hChild = GetChildItem(pNMTreeView->itemNew.hItem);
        }
        InsertItem("", pNMTreeView->itemNew.hItem);
        SetItemData(pNMTreeView->itemNew.hItem, LOADDIR);
      }
    }
  }
  *pResult = 0;
}


void CFileTree::ExpandItem(HTREEITEM hItem, UINT nCode)
{
  CString strPath;
  if (nCode == TVE_EXPAND)
  {
    if (hItem != GetRootItem())
    {
      int nData = GetItemData(hItem);
      if (nData != LOADDIR)				//未加载子目录，开始加载子目录
      {
        HTREEITEM hChild = GetChildItem(hItem);
        DeleteItem(hChild);
        strPath = GetFullPath(hItem);
        ShowPath(hItem, strPath);
        SetItemData(hItem, LOADDIR);	//已加载目录
      }
      else								//已加载目录，判断是否有新的目录或文件需要加载
      {
        CString csPath = "";
        csPath = GetFullPath(hItem);
        char chDir[MAX_PATH] = { 0 };
        strcpy(chDir, csPath);
        //获取新的目录信息
        CFileFind flFind;
        BOOL      bFind;
        CSortString strDirArray;
        strcat(chDir, "\\*.*");

        bFind = flFind.FindFile(chDir);
        while (bFind)
        {
          bFind = flFind.FindNextFile();
          if (!flFind.IsDots())
          {
            strDirArray.Add(flFind.GetFilePath());
          }
        }
        strDirArray.Sort();
        //获取当前节点下的子目录
        CSortString csNodeArray;
        HTREEITEM hChild = GetChildItem(hItem);
        while (hChild)
        {
          csNodeArray.Add(GetItemText(hChild));
          hChild = GetNextItem(hChild, TVGN_NEXT);
        }
        csNodeArray.Sort();
        int nSize = csNodeArray.GetSize();
        int *pArray = new int[nSize];
        memset(pArray, -1, nSize);
        int nCount = 0;
        //将磁盘的目录信息与节点信息进行匹配，查找差异目录，将节点中没有显示的新的磁盘目录添加到节点中
        //将节点中有磁盘目录不符的节点目录删除
        for (int i = 0; i < strDirArray.GetSize(); i++)
        {
          BOOL bExist = FALSE;
          for (int j = 0; j < nSize; j++)
          {
            CString csDisk = ExtractPath(strDirArray.GetAt(i));
            CString csNode = csNodeArray.GetAt(j);
            if (csDisk == csNode)	//节点与目录进行匹配
            {
              bExist = TRUE;
              pArray[nCount] = j;	//记录匹配的元素
              nCount++;
              break;
            }
          }
          if (!bExist)				//目录不存在,在结尾添加目录
          {

            HTREEITEM hNode = AddItem(hItem, strDirArray.GetAt(i));
            //获取父节点的状态
            UINT nState = GetItemState(hItem, TVIS_STATEIMAGEMASK) >> 12;
            nState = (nState == 2) ? 3 : nState;
            SetItemState(hNode, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
          }
        }
        //在csNodeArray中查找不存在的目录
        int *pNoMatch = new int[nSize];
        memset(pNoMatch, -1, nSize);
        int nMatchCounter = 0;
        for (int nIndex = 0; nIndex < nSize; nIndex++)
        {
          BOOL bMatched = FALSE;
          for (int nID = 0; nID < nCount; nID++)
          {
            if (nIndex == pArray[nID])
            {
              bMatched = TRUE;
              break;
            }
          }
          if (!bMatched)
          {
            pNoMatch[nMatchCounter] = nIndex;
            nMatchCounter++;
          }
        }
        if (nMatchCounter > 0)	//删除节点中不存在的目录信息
        {
          for (int i = 0; i < nMatchCounter; i++)
          {
            HTREEITEM hChildItem = FindNode(hItem, csNodeArray.GetAt(pNoMatch[i]));
            if (hChildItem)
            {

              UINT nState = GetItemState(hChildItem, TVIS_STATEIMAGEMASK) >> 12;
              nState = (nState == 3) ? 1 : 3;
              DeleteItem(hChildItem);
              SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
            }
          }
        }
        delete[] pArray;
        delete[] pNoMatch;
      }
    }
  }
}

HTREEITEM CFileTree::FindNode(HTREEITEM hParant, LPCTSTR lpszName)
{
  HTREEITEM hChild = GetChildItem(hParant);
  CString csText = GetItemText(hChild);
  if (lpszName == csText)
  {
    return hChild;
  }
  while (hChild)
  {
    hChild = GetNextItem(hChild, TVGN_NEXT);
    csText = GetItemText(hChild);
    if (lpszName == csText)
    {
      return hChild;
    }
  }
  return NULL;
}


