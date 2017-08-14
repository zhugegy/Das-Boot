// DasBootDllRemoteCmd.h : main header file for the DasBootDllRemoteCmd DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDasBootDllRemoteCmdApp
// See DasBootDllRemoteCmd.cpp for the implementation of this class
//

class CDasBootDllRemoteCmdApp : public CWinApp
{
public:
	CDasBootDllRemoteCmdApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
