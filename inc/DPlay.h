/////////////////////////////////////////////////////////////////////////////
//		File:			DPlay.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:52:18 AM
//
//		Description:	Wrapper class (for what used to be DirectPlay, hence
//						the filename DPlay) for HawkNL networking library.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Singleton.h"
#include "nl.h"

#include "ClientSocket.h"
#include "ServerSocket.h"

//////////////////////////////////////////////////////////////////////////

extern CRITICAL_SECTION	m_CS;

//////////////////////////////////////////////////////////////////////////

class DPlay : public Singleton<DPlay>
{
private:
	CServerSocket*	m_Server;
	CClientSocket*	m_Client;

	Bool			m_bIsServer;
	Bool			m_bInitialized;

private:
	Int32				ServerProc();
	Int32				ClientProc();

public:
	DPlay(void);
	virtual ~DPlay(void);

	Int32 Init(Bool IsServer = false);
	Int32 Update();
	Int32 Shutdown();

	Int32			Connect();

	Bool		IsInitialized()	{return m_bInitialized; }

	CClientSocket* GetClient() {return m_Client;}

public:
#ifdef _DOOMEROIDS_
	PlayerPtr	m_PlayerList[5];
#endif
};

#define g_DPlay		DPlay::GetInstance()