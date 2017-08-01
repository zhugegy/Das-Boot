#include <string>
#include <list>
#include <unordered_map>

using namespace std;

typedef int(*pfnDBExport)(string &strParam, list<string>& lstMsgToSend);

typedef int(*pfnPluginInterface)(unordered_map<string, pfnDBExport>& mapFunctions);

extern "C" {
  __declspec(dllexport) int dbplugin_load_dll(unordered_map<string, pfnDBExport>& mapFunctions);
}

int db_hello_world(string &strParam, list<string>& lstMsgToSend)
{
  //TRACE(strParam.GetBuffer());

  printf("hello world from dll.\r\n");

  printf("%s", strParam.c_str());

  return 0;
}

//所有的dll必须有，并且导出此函数
int dbplugin_load_dll(unordered_map<string, pfnDBExport>& mapFunctions)
{
  mapFunctions["HelloWorldTempl"] = (pfnDBExport)db_hello_world;

  //TRACE(_T("本dll内的所有所有函数加载成功"));
  return 0;
}
