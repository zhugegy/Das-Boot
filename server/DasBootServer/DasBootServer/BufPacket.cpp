#include "stdafx.h"
#include "BufPacket.h"

#include "DasBootServer.h"

CBufPacket::CBufPacket()
{
  m_pchBuf = NULL;
  m_nSize = 0;
  m_nLength = 0;

  InitBuf();
}


CBufPacket::~CBufPacket()
{
  ReleaseBuf();
}

CBufPacket::CBufPacket(CBufPacket& pkt)
{
  char* pNewBuf = NULL;
  m_nSize = pkt.m_nSize;
  m_nLength = pkt.m_nLength;

  //深拷贝
  if (m_nSize > 0)
  {
    pNewBuf = new char[m_nSize];
    memcpy(pNewBuf, pkt.m_pchBuf, m_nSize);
  }

  if (m_pchBuf != NULL)
  {
    delete[] m_pchBuf;
    m_pchBuf = NULL;
  }

  m_pchBuf = pNewBuf;
}

BOOL CBufPacket::InitBuf()
{
  if (m_pchBuf != NULL)
  {
    delete[] m_pchBuf;
    m_pchBuf = NULL;
  }

  m_pchBuf = new char[BUF_SIZE];

  if (m_pchBuf == NULL)
  {
    return FALSE;
  }

  m_nSize = BUF_SIZE;
  m_nLength = 0;

  return TRUE;
}

BOOL CBufPacket::ReleaseBuf()
{
  if (m_pchBuf != NULL)
  {
    delete[] m_pchBuf;
    m_pchBuf = NULL;
  }

  m_nSize = 0;
  m_nLength = 0;

  return TRUE;
}


BOOL CBufPacket::SetBuf(char* pszBuf, int nBufSize, int nLength)
{
  if (m_pchBuf != NULL)
  {
    delete[] m_pchBuf;
  }

  m_pchBuf = pszBuf;
  m_nSize = nBufSize;
  m_nLength = nLength;

  return TRUE;
}


//获取缓冲区
char* CBufPacket::GetBuf() 
{
  return m_pchBuf;
}

//获取缓冲区长度
int CBufPacket::GetLength() 
{
  return m_nLength;
}

//将数据添加到缓冲区中
BOOL CBufPacket::Append(char* pBuf, int nLength)
{
  if (m_pchBuf == NULL)
  {
    return FALSE;
  }

  //缓冲区长度不够
  if (m_nSize < m_nLength + nLength)
  {
    //重新分配空间
    int nNewSize = (m_nLength + nLength) * BUF_FACTOR;
    char* pNewBuf = new char[nNewSize];

    //拷贝原有的数据
    memcpy(pNewBuf, m_pchBuf, m_nLength);

    delete[] m_pchBuf;
    m_nSize = nNewSize;
    m_pchBuf = pNewBuf;
  }

  //缓冲区长度够用
  memcpy(m_pchBuf + m_nLength, pBuf, nLength);
  m_nLength += nLength;

  return TRUE;
}

extern CDasBootServerApp theApp;

//自处理
BOOL CBufPacket::Handle(SOCKET s)
{
  //检查缓冲区长度够不够
  if (m_nLength < 4)
  {
    return FALSE;
  }

  int nRequiredSize = *(int*)m_pchBuf;

  //表明包没有接收完整,仍然不处理
  if (m_nLength < nRequiredSize)
  {
    return FALSE;
  }

  //进入处理的代码, 处理数据
  SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PKTHANDLEMSG, (WPARAM)s,
    (LPARAM)m_pchBuf);

  //数据处理完毕,如果后面有多余的数据,则往前挪动
  memmove(m_pchBuf, m_pchBuf + nRequiredSize, m_nLength - nRequiredSize);

  m_nLength -= nRequiredSize;

  return TRUE;
}