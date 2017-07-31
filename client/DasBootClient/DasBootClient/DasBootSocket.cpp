#include "DasBootSocket.h"


CDasBootSocket::CDasBootSocket(SOCKET hSocket)
{
  m_Socket = hSocket;
}


CDasBootSocket::~CDasBootSocket()
{
}

int CDasBootSocket::SendPkt(CBufPacket& pkt)
{

  return SendData(pkt.GetBuf(), pkt.GetLength());
}

//undone 这里是重置了pkt的信息，比较暴力，暂时先这么用。真正的大批量处理数据，都是用
//CBufPacket里面的Append()和Handle()函数。所以说，这里需要进一步处理，但是暂时先这么
//放着。
//此函数，只可以用于小批量数据的接受（例如Client里接受Server回复信息），不能处理大量
//连续数据。
//换句话说，这个函数，只适用于没有套用网络模型的客户端程序，用于进行简单地接受消息。
int CDasBootSocket::RecvPkt(CBufPacket& pkt)
{
  char szHead[5] = { 0 };

  //首先接收4个字节的头部,表示当前数据包的大小
  int nRet = RecvData(szHead, 4);
  if (nRet != SOCKET_SUCCESS)
  {
    return nRet;
  }
   
  //表示已经成功的接收了5个字节的头部.
  int nRequreLength = *(DWORD*)(szHead);


  char* pszBuf = new char[nRequreLength];
  if (pszBuf == NULL)
  {
    return SOCKET_ERROR;
  }

  (DWORD&)*pszBuf = nRequreLength;

  nRet = RecvData(pszBuf + 4, nRequreLength - 4);

  if (nRet == 1)
  {
    pkt.SetBuf(pszBuf, nRequreLength, nRequreLength);
    return SOCKET_SUCCESS;
  }
  else
  {
    delete pszBuf;
  }

  return SOCKET_ERROR;
}

//发送指定大小的数据
int CDasBootSocket::SendData(const char* pszData, int nLength)
{
  if (pszData == NULL)
  {
    return 0;
  }

  int nTotalSentBytes = 0;
  while (nTotalSentBytes < nLength)
  {
    int nSendedBytes = send(m_Socket, pszData + nTotalSentBytes,
      nLength - nTotalSentBytes, 0);

    if (nSendedBytes == SOCKET_ERROR)
    {
      return nSendedBytes;
    }

    nTotalSentBytes += nSendedBytes;
  }

  return SOCKET_SUCCESS;
}

//接收指定大小的数据
int CDasBootSocket::RecvData(char* pszData, int nLength)
{
  if (pszData == NULL)
  {
    return SOCKET_ERROR;
  }

  int nTotalRecvedBytes = 0;
  while (nTotalRecvedBytes < nLength)
  {
    int nRecvedBytes = recv(m_Socket, pszData + nTotalRecvedBytes,
      nLength - nTotalRecvedBytes, 0);

    if (nRecvedBytes == SOCKET_ERROR)
    {
      return nRecvedBytes;
    }
    else if (nRecvedBytes == 0) //表示对方"优雅"关闭了socket
    {
      return 0;
    }

    nTotalRecvedBytes += nRecvedBytes;
  }

  return SOCKET_SUCCESS;
}