#include "LoadDll.h"
#include <stdlib.h>
//#include <Windows.h>
#include <stdio.h>

#include <string>
#include <unordered_map>

#include "DasBootClient.h"

#define MAXBYTE     0xff 

int LoadExsitingDllWhenStartUp()
{
  char szPath[MAXBYTE] = { 0 };

  DWORD dwRet = GetModuleFileName(NULL, szPath, MAXBYTE);

  char szDrive[_MAX_DRIVE];
  char szDir[_MAX_DIR];
  char szFname[_MAX_FNAME];
  char szExt[_MAX_EXT];

  _splitpath(szPath, szDrive, szDir, szFname, szExt);

  char szDirectoryToEnum[MAXBYTE];

  sprintf(szDirectoryToEnum, "%s%s*.dll", szDrive, szDir);

  WIN32_FIND_DATA FindFileData;
  HANDLE hFind;

  hFind = FindFirstFile(szDirectoryToEnum, &FindFileData);

  while (hFind != INVALID_HANDLE_VALUE)
  {
    DasBootClientLoadDll(FindFileData.cFileName);

    if (!FindNextFile(hFind, &FindFileData))
    {
      break;
    }
  }

  if (hFind != INVALID_HANDLE_VALUE)
  {
    FindClose(hFind);
  }

  return 0;
}

extern std::unordered_map<std::string, pfnDBExportClient> g_mapFunctions;

int DasBootClientLoadDll(const char *szDllPath)
{
  HINSTANCE hDll = LoadLibrary(szDllPath);

  if (hDll == NULL)
  {
    //TRACE(_T("DLLº”‘ÿ ß∞‹"));
    return 0;
  }

  pfnPluginInterfaceClient pfnInterface = 
    (pfnPluginInterfaceClient)GetProcAddress(hDll, "DBLoadDLLClient");

  pfnInterface(g_mapFunctions);

  return 0;
}