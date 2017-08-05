// DasBootDllHelloWorld.h : main header file for the DasBootDllHelloWorld DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDasBootDllHelloWorldApp
// See DasBootDllHelloWorld.cpp for the implementation of this class
//

class CDasBootDllHelloWorldApp : public CWinApp
{
public:
	CDasBootDllHelloWorldApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
