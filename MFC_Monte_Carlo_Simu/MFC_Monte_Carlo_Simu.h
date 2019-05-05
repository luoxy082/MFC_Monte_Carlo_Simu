
// MFC_Monte_Carlo_Simu.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMFCMonteCarloSimuApp:
// See MFC_Monte_Carlo_Simu.cpp for the implementation of this class
//

class CMFCMonteCarloSimuApp : public CWinApp
{
public:
	CMFCMonteCarloSimuApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMFCMonteCarloSimuApp theApp;
