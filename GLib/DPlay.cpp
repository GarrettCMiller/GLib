/////////////////////////////////////////////////////////////////////////////
//		File:			DPlay.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:52:01 AM
//
//		Description:	Wrapper class (for what used to be DirectPlay, hence
//						the filename DPlay) for HawkNL networking library.
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\dplay.h"

#include <objbase.h>

#include "Globals.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(DPlay);

CRITICAL_SECTION	m_CS;

//////////////////////////////////////////////////////////////////////////

DPlay::DPlay(void) :
	m_bInitialized	(false),
	m_bIsServer		(false),
	m_Server		(NULL),
	m_Client		(NULL)
{
#ifdef _DOOMEROIDS_
	ZeroMemory(m_PlayerList, sizeof(Player*) * 5);
#endif
}

//////////////////////////////////////////////////////////////////////////

DPlay::~DPlay(void)
{
	if (!Shutdown())
	{
		g_Log.LOG_ERROR("Error shutting HawkNL down!!\n");
	}
}

//////////////////////////////////////////////////////////////////////////

Int32 DPlay::Init(Bool IsServer)
{
	if (IsInitialized())
		return -1;

	if (IsServer)
	{
		g_NetLog.OpenNewLog("Network.html");
		g_NetLog.SetMode(eLM_UList);
	}
	else
	{
		g_NetLog.OpenNewLog("Network Client.html");
		g_NetLog.SetMode(eLM_UList);
	}

	g_Log << "DPlay::Init() begin...\n";
	g_NetLog << "DPlay::Init() begin...\n";

	m_bIsServer = IsServer;

	/*if (!nlInit())
	{
		g_Log.LOG_ERROR("Error initializing HawkNL!!\n");
		return 0;
	}*/

	/*g_NetLog	<< "nlGetString(NL_VERSION) = "			<< nlGetString(NL_VERSION)			<< "\n"
				<< "nlGetString(NL_NETWORK_TYPES) = "	<< nlGetString(NL_NETWORK_TYPES)	<< "\n";*/

	/*if (!nlSelectNetwork(NL_IP))
	{
		g_Log.LOG_ERROR("Error selecting network!!\n");
		return 0;
	}*/

	if (m_bIsServer)
		m_Server = new CServerSocket;
	
	m_Client = new CClientSocket;

	m_bInitialized = true;
	
	g_Log << "DPlay::Init() ends!\n";
	g_NetLog << "DPlay::Init() ends!\n";

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 DPlay::Connect()
{
	if (m_bIsServer)
		if (m_Server->Init() <= 0)
			return -1;
	
	if (m_Client->Init() <= 0)
		return 0;

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 DPlay::Update()
{
	Int32 ret = 0;

	if (m_bIsServer && m_Server)
		ServerProc();
	
	if (m_Client)
		ret = ClientProc();

	return ret;
}

//////////////////////////////////////////////////////////////////////////

Int32 DPlay::ClientProc()
{
	return m_Client->Update();
}

//////////////////////////////////////////////////////////////////////////

Int32 DPlay::ServerProc()
{
	return m_Server->Update();
}

//////////////////////////////////////////////////////////////////////////

Int32 DPlay::Shutdown()
{
	g_Log << "DPlay::Shutdown() begin...\n";
	g_NetLog << "DPlay::Shutdown() begin...\n";

	delete m_Server;
	m_Server = NULL;

	delete m_Client;
	m_Client = NULL;

	//nlShutdown();

	m_bInitialized = false;

	g_Log << "DPlay::Shutdown() end!\n";
	g_NetLog << "DPlay::Shutdown() end!\n";

	return 1;
}

//////////////////////////////////////////////////////////////////////////