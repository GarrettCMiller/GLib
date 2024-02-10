///////////////////////////////////////////////////////////////////////////
//		File:				ClientSocket.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Implementation of a client socket
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "MySocket.h"

//////////////////////////////////////////////////////////////////////////

class CClientSocket : public CMySocket
{
	static std::string 		m_ServersAddress;

public:
	CClientSocket(void);
	virtual ~CClientSocket(void);

	Int32	Init();
	Int32 Update();
	Int32 Shutdown();
};
