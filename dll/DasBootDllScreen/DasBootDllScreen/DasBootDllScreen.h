// DasBootDllScreen.h : main header file for the DasBootDllScreen DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDasBootDllScreenApp
// See DasBootDllScreen.cpp for the implementation of this class
//

class CDasBootDllScreenApp : public CWinApp
{
public:
	CDasBootDllScreenApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
