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

//using namespace std;


static int DBBCClientListInfoQueryM(const char *strParam, SOCKET hSocket);
static char** str_split(char* a_str, const char a_delim);


typedef int(*pfnDBExportServer)(const char *strParam, SOCKET hClientSocket);

extern std::unordered_map<std::string, pfnDBExportServer> g_mapFunctions;
extern CDasBootServerDlg* g_pMainDlg;

int SendMessageOut(SOCKET hSocket, const char * szType, const char * szContent)
{
  CDasBootSocket sendSkt(hSocket);
  CBufPacket pkt;

  char szHead[4] = { 0 };
  DWORD dwSize = sizeof(DWORD) + 16 + strlen(szContent) + 1;
  (DWORD&)*szHead = dwSize;
  pkt.Append(szHead, sizeof(szHead));
  pkt.Append((char *)szType, 16);
  pkt.Append((char *)szContent, strlen(szContent) + 1);

  sendSkt.SendPkt(pkt);

  return 0;
}

int AddBasicCommandsToMapFunctions()
{
  g_mapFunctions["ClientListInfoR"] = (pfnDBExportServer)DBBCClientListInfoQueryM;

  return 0;
}


int DBBCClientListInfoQueryC(SOCKET hSocket)
{
  SendMessageOut(hSocket, _T("ClientListInfoC"), _T("nothing"));

  return 0;
}

int DBBCClientListInfoQueryM(const char *strParam, SOCKET hSocket)
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