// World.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "World.h"
#include "WorldDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWorldApp

BEGIN_MESSAGE_MAP(CWorldApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWorldApp construction

CWorldApp::CWorldApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CWorldApp object

CWorldApp theApp;


// CWorldApp initialization

BOOL CWorldApp::InitInstance()
{
	AfxEnableControlContainer();

	CWorldDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	return FALSE;
}
