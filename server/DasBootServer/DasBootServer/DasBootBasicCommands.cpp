#include "stdafx.h"
#include "DasBootBasicCommands.h"
#include "BufPacket.h"
#include "DasBootSocket.h"
#include "DasBootServerDlg.h"
#include <string>
#include <unordered_map>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "GeoLite2PP.hpp"

#include "ClientManager.h"
#include "DasBootServer.h"

//using namespace std;


static int DBBMClientListInfoQueryM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
static int DBBMGiveClientFileM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
static int DBBMTranClientFileM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
static int DBBMClosClientFileM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
static int DBBMGetClientFileM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
static int DBBMCloseServerFileM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);



static int TransferFile(SOCKET hSocket, const char *szFileName, const char *szMsgType);
static char** str_split(char* a_str, const char a_delim);

extern std::unordered_map<std::string, pfnDBExportServer> g_mapFunctions;
extern CDasBootServerDlg* g_pMainDlg;

int SendMessageOut(SOCKET hSocket, const char * szType, const char * szContent, DWORD dwContentLength)
{
  CDasBootSocket sendSkt(hSocket);
  CBufPacket pkt;
  DWORD dwMsgContentLength = 0;

  if (dwContentLength == -1)
  {
    dwMsgContentLength = strlen(szContent) + 1;
  }
  else
  {
    dwMsgContentLength = dwContentLength;
  }

  char szHead[4] = { 0 };

  DWORD dwSize = sizeof(DWORD) + 16 + dwMsgContentLength;
  (DWORD&)*szHead = dwSize;
  pkt.Append(szHead, sizeof(szHead));
  pkt.Append((char *)szType, 16);
  pkt.Append((char *)szContent, dwMsgContentLength);

  sendSkt.SendPkt(pkt);

  return 0;
}


int AddBasicCommandsToMapFunctions()
{
  g_mapFunctions["ClientListInfoR"] = (pfnDBExportServer)DBBMClientListInfoQueryM;
  g_mapFunctions["GiveClientFileR"] = (pfnDBExportServer)DBBMGiveClientFileM;
  g_mapFunctions["TranClientFileR"] = (pfnDBExportServer)DBBMTranClientFileM;
  g_mapFunctions["ClosClientFileR"] = (pfnDBExportServer)DBBMClosClientFileM;
  g_mapFunctions["GetClientFile0R"] = (pfnDBExportServer)DBBMGetClientFileM;
  g_mapFunctions["ClosServerFileN"] = (pfnDBExportServer)DBBMCloseServerFileM;

  return 0;
}


int DBBCClientListInfoQueryC(SOCKET hSocket)
{
  SendMessageOut(hSocket, _T("ClientListInfoC"), _T("nothing"));

  return 0;
}

int DBBCGiveClientFileC(SOCKET hSocket, const char * szServerFileLocation,
  const char * szClientFileLocation)
{
  char *pFileInfo = new char[strlen(szClientFileLocation) + 
    strlen(szServerFileLocation) + 1 + 1];

  sprintf(pFileInfo, "%s%c%s", szServerFileLocation, '\1', 
    szClientFileLocation);

  SendMessageOut(hSocket, _T("GiveClientFileC"), pFileInfo);

  delete[] pFileInfo;

  return 0;
}

extern CDasBootServerApp theApp;

int DBBCGetClientFileC(SOCKET hSocket, const char * szServerFileLocation,
  const char * szClientFileLocation)
{
  CClientContext *pClientContext = NULL;
  theApp.m_objClientManager.FindAt(hSocket, pClientContext);

  pClientContext->m_pCurrentFile = fopen(szServerFileLocation, "ab");
  pClientContext->m_nFileChunckCount = 0;

  SendMessageOut(hSocket, _T("GetClientFile0C"), szClientFileLocation);

  return 0;
}

int DBBMClientListInfoQueryM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  int nClientListCount = g_pMainDlg->m_lstctlClientList.GetItemCount();

  for (int i = 0; i < nClientListCount; i++)
  {
    if (hSocket == g_pMainDlg->m_lstctlClientList.GetItemData(i))
    {
      char** tokens;
      tokens = str_split((char *)strParam, '\1');

      if (tokens)
      {
        int nStep = 1;
        for (int j = 0; *(tokens + j); j++)
        {
          if (j == 1)
          {
            nStep += 1;
          }

          g_pMainDlg->m_lstctlClientList.SetItemText(i, j + nStep, 
            *(tokens + j));

          free(*(tokens + j));
        }
        free(tokens);
      }//if (tokens)

      //如果有有效IP，则设置地理信息
      if (strcmp(
        (g_pMainDlg->m_lstctlClientList.GetItemText(i, 1)).GetBuffer(0), 
        _T("unkown-timeout")
                 ) != 0)
      {
        GeoLite2PP::DB db("GeoLite2-City.mmdb");

        std::string strIP(
          (LPCTSTR)g_pMainDlg->m_lstctlClientList.GetItemText(i, 1));

        //strIP.pop_back();  //去除最后的'\n'(在客户端发送之前去除了)

        std::string strCountry = db.get_field(strIP, "en", 
          GeoLite2PP::VCStr{"country", "names" });
        std::string strCity = db.get_field(strIP, "en", 
          GeoLite2PP::VCStr{"city", "names" });

        CString strGeo;
        strGeo.Format(_T("%s, %s"), strCountry.c_str(), strCity.c_str());

        g_pMainDlg->m_lstctlClientList.SetItemText(i, 2, strGeo);
      }

    }//if (hSocket == g_pMainDlg->m_lstctlClientList.GetItemData(i))
  }//for (int i = 0; i < nClientListCount; i++)

  //DBBCGetClientFileC(hSocket, _T("okjieshou.flv"), _T("C:\\Users\\Administrator\\Desktop\\chidouren.flv"));

  return 0;
}

//https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
char** str_split(char* a_str, const char a_delim)
{
  char** result = 0;
  size_t count = 0;
  char* tmp = a_str;
  char* last_comma = 0;
  char delim[2];
  delim[0] = a_delim;
  delim[1] = 0;

  /* Count how many elements will be extracted. */
  while (*tmp)
  {
    if (a_delim == *tmp)
    {
      count++;
      last_comma = tmp;
    }
    tmp++;
  }

  /* Add space for trailing token. */
  count += last_comma < (a_str + strlen(a_str) - 1);

  /* Add space for terminating null string so caller
  knows where the list of returned strings ends. */
  count++;

  result = (char **)malloc(sizeof(char*) * count);

  if (result)
  {
    size_t idx = 0;
    char* token = strtok(a_str, delim);

    while (token)
    {
      assert(idx < count);
      *(result + idx++) = strdup(token);
      token = strtok(0, delim);
    }
    assert(idx == count - 1);
    *(result + idx) = 0;
  }

  return result;
}

int DBBMGiveClientFileM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  //开始传输文件
  TransferFile(hSocket, strParam, TEXT("TranClientFileC"));


  return 0;
}

int DBBMTranClientFileM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  //目前文件传输的计数器（1024byte为单位）
  int nCount = (int &)(*strParam);
  TRACE("当前KB %d", nCount);

  return 0;
}

int DBBMClosClientFileM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  //传输成功
  TRACE("FILE TRANSFER SUCCESS");


  return 0;
}

int DBBMGetClientFileM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  fwrite(strParam, 1, nMsgLength - 4 - 16, pClientContext->m_pCurrentFile);
  pClientContext->m_nFileChunckCount++;

  TRACE("目前接受KB %d", pClientContext->m_nFileChunckCount);
  return 0;
}

int DBBMCloseServerFileM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  fflush(pClientContext->m_pCurrentFile);
  fclose(pClientContext->m_pCurrentFile);
  pClientContext->m_nFileChunckCount = 0;

  TRACE("接受客户端文件成功");

  return 0;
}

//https://codereview.stackexchange.com/questions/43914/client-server-implementation-in-c-sending-data-files
int TransferFile(SOCKET hSocket, const char *szFileName, const char *szMsgType)
{
  FILE *pFile = fopen(szFileName , "rb");
  if (pFile == NULL)
  {
    //printf("File opern error");
    return 1;
  }

  while (1)
  {
    /* First read file in chunks of 256 bytes */
    unsigned char buff[1024] = { 0 };
    int nread = fread(buff, 1, 1024, pFile);
    //printf("Bytes read %d \n", nread);

    /* If read was success, send data. */
    if (nread > 0)
    {
      //printf("Sending \n");
      //write(connfd, buff, nread);

      SendMessageOut(hSocket, szMsgType, (const char *)buff, nread);
    }

    /*
    * There is something tricky going on with read ..
    * Either there was error, or we reached end of file.
    */
    if (nread < 1024)
    {
      if (feof(pFile))
      {
        //printf("End of file\n");

      }

      if (ferror(pFile))
      {
        //printf("Error reading\n");
      }

      break;
    }
  }//while (1)

  fclose(pFile);

  SendMessageOut(hSocket, _T("ClosClientFileC"), TEXT("nothing(current)"));


  return 0;
}

