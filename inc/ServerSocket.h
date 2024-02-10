///////////////////////////////////////////////////////////////////////////
//		File:				ServerSocket.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Implementation of a server socket
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "MySocket.h"
#include "Timer.h"
//////////////////////////////////////////////////////////////////////////

class CServerSocket : public CMySocket
{
	NLsocket	m_Client[10];
	Int32			m_NumClients;
	Int32			m_Group;
	Timer		m_ErrorTimer;

	Int32			CheckForNewClients();

public:
	CServerSocket(void);
	virtual ~CServerSocket(void);

	Int32	Init();
	Int32 Update();
	Int32 Shutdown();
};
