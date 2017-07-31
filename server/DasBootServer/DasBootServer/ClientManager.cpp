#include "stdafx.h"
#include "ClientManager.h"


CClientManager::CClientManager()
{
}


CClientManager::~CClientManager()
{
  SOCKET s = INVALID_SOCKET;
  CClientContext* pClient = NULL;

  POSITION pos = m_Map.GetStartPosition();

  while (pos)
  {
    m_Map.GetNextAssoc(pos, s, pClient);
    if (pClient != NULL)
    {
      delete pClient;
    }
  }

  m_Map.RemoveAll();
}

//增
BOOL CClientManager::Add(SOCKET s, CClientContext* pClient)
{

  m_Map.SetAt(s, pClient);

  return TRUE;
}

//删
BOOL CClientManager::Delete(SOCKET s)
{
  CClientContext* pClient;

  m_Map.Lookup(s, pClient);

  if (pClient != NULL)
  {
    delete pClient;
  }

  m_Map.RemoveKey(s);

  return TRUE;
}

//查
BOOL CClientManager::FindAt(SOCKET s, CClientContext*& pClient)
{
  m_Map.Lookup(s, pClient);

  return TRUE;
}

//遍历并干活
BOOL CClientManager::IterateAndDoSomething(PFNTransferMsg pfn, CString& str)
{
  SOCKET s = INVALID_SOCKET;
  CClientContext* pClient = NULL;

  POSITION pos = m_Map.GetStartPosition();

  while (pos)
  {
    m_Map.GetNextAssoc(pos, s, pClient);
    if (pClient != NULL)
    {
      //开始干活
      pClient->DoSomething(pfn, str);
    }
  }

  return TRUE;
}