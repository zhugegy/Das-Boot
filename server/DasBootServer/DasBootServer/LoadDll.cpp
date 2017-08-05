#include "stdafx.h"
#include "LoadDll.h"
#include "DasBootServer.h"

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
    DasBootServerLoadDll(FindFileData.cFileName);

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

extern std::unordered_map<std::string, pfnDBExportServer> g_mapFunctions;

int DasBootServerLoadDll(const char *szDllPath)
{
  HINSTANCE hDll = LoadLibrary(szDllPath);

  if (hDll == NULL)
  {
    //TRACE(_T("DLLº”‘ÿ ß∞‹"));
    return 0;
  }

  pfnPluginInterfaceServer pfnInterface = (pfnPluginInterfaceServer)GetProcAddress(hDll,
    "DBLoadDLLServer");

  pfnInterface(theApp.m_mapConfirmFunctions, g_mapFunctions, theApp.m_mapDllIntroduction);

  char szDrive[_MAX_DRIVE];
  char szDir[_MAX_DIR];
  char szFname[_MAX_FNAME];
  char szExt[_MAX_EXT];
  _splitpath(szDllPath, szDrive, szDir, szFname, szExt);

  CString strDllName;
  strDllName.Format("%s%s", szFname, szExt);
  theApp.m_lstCurrentDllNames.AddTail(strDllName);

  return 0;
}