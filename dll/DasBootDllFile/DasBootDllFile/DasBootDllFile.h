// DasBootDllFile.h : main header file for the DasBootDllFile DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDasBootDllFileApp
// See DasBootDllFile.cpp for the implementation of this class
//

class CDasBootDllFileApp : public CWinApp
{
public:
	CDasBootDllFileApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
