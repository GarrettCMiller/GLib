#pragma once

//#include "stdafx.h"

#include "Functor.h"
#include "Vec.h"

class CThread;

// CGLibMFCApp

class CGLibMFCApp : public CWinApp
{
	DECLARE_DYNCREATE(CGLibMFCApp)

protected:
	CGLibMFCApp();           // protected constructor used by dynamic creation
	virtual ~CGLibMFCApp();

public:
	//typedef CStaticThreadFunctor TFunctor;
	typedef COpenFunctor<DWORD, LPVOID>	TFunctor;
	
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	virtual void BeginThread();

	void SetInitFunc(TFunctor& pfnMI)
	{
		pfnMainInit = pfnMI;
	}
	void SetUpdateFunc(TFunctor& pfnMU)
	{
		pfnMainUpdate = pfnMU;
	}

protected:
	DECLARE_MESSAGE_MAP()
	
	//The main update loop for the app
	static DWORD _MainUpdateLoop(LPVOID data);
	static DWORD _MainInit(LPVOID data);

public:
	static Bool		m_bOverrideMain;

	//Vars to handle to separate thread quitting properly
	static Bool		m_bActive;
	static Bool		m_bQuit;
	static Bool		m_bInLoop;
	static CThread*	m_pThread;

	static Vec2u	m_vBufferSize;

	static TFunctor	pfnMainInit;
	static TFunctor	pfnMainUpdate;

};


