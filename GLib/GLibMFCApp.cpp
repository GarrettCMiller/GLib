// GLibMFCApp.cpp : implementation file
//

#include "stdafx.h"
#include "GLib.h"
#include "GLibMFCApp.h"

#include "D3DWrapper.h"
#include "ThreadMgr.h"
#include "EffectMgr.h"

// CGLibMFCApp

CThread* CGLibMFCApp::m_pThread			= NULL;

Bool	CGLibMFCApp::m_bActive			= false;
Bool	CGLibMFCApp::m_bQuit			= false;
Bool	CGLibMFCApp::m_bInLoop			= false;

Bool	CGLibMFCApp::m_bOverrideMain	= false;

Vec2u	CGLibMFCApp::m_vBufferSize		= Vec2u(1024, 768);

CGLibMFCApp::TFunctor	CGLibMFCApp::pfnMainInit;
CGLibMFCApp::TFunctor	CGLibMFCApp::pfnMainUpdate;


IMPLEMENT_DYNCREATE(CGLibMFCApp, CWinApp)

CGLibMFCApp::CGLibMFCApp()
{
}

CGLibMFCApp::~CGLibMFCApp()
{
	//Tell the loop to quit
	m_bQuit = true;

	//While the threads are running, tell them to quit
	while (!g_ThreadMgr->AllThreadsKilled(true) || m_bInLoop)
	{
		Sleep(30);
	}

	//Make sure to kill the thread manager and d3d
	g_ThreadMgr->DeleteInstance();
	g_EffectMgr->DeleteInstance();
	g_D3D->DeleteInstance();
}

BOOL CGLibMFCApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

int CGLibMFCApp::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinApp::ExitInstance();
}

BEGIN_MESSAGE_MAP(CGLibMFCApp, CWinApp)
END_MESSAGE_MAP()


// CGLibMFCApp message handlers
void CGLibMFCApp::BeginThread()
{
	g_ThreadMgr->CreateThread(LPTHREAD_START_ROUTINE(&CGLibMFCApp::_MainUpdateLoop), NULL, true, &m_pThread);

	m_bActive = true;

}

DWORD CGLibMFCApp::_MainUpdateLoop(LPVOID data)
{
	if (m_bOverrideMain)
		pfnMainUpdate(data);
	else
	{
		try
		{
			if (pfnMainInit)
				pfnMainInit(data);
		}
		catch (CException* e)
		{
			e->ReportError();
			e->Delete();
			return -1;
		}

		while (!m_bQuit)
		{
			try
			{
				if (m_bActive)
				{
					m_bInLoop = true;

					pfnMainUpdate(data);

					m_bInLoop = false;
				}
			}
			catch (CException* e)
			{
				e->ReportError();
				e->Delete();
				return -2;
			}
		}
	}

	//Just in case
	m_bInLoop = false;

	m_pThread->Shutdown();

	return 0;
}