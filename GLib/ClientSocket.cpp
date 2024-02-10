///////////////////////////////////////////////////////////////////////////
//		File:				ClientSocket.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Implementation of a client socket
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "ClientSocket.h"
//#include "Player.h"
#include "SpriteMgr.h"
#include "DPlay.h"
//#include "MapMgr.h"

#include "Globals.h"

//////////////////////////////////////////////////////////////////////////

String  CClientSocket::m_ServersAddress = 
//#ifdef _DEBUG
	"127.0.0.1:25000";
//#else
//	"172.31.103.33:25000";
//#endif

//////////////////////////////////////////////////////////////////////////

CClientSocket::CClientSocket(void)
{
	//ZeroMemory(&m_PlayerData, sizeof(PlayerData));
	
	//m_PlayerData.PlayerID = -1;		//So we know we haven't connected yet.
}

//////////////////////////////////////////////////////////////////////////

CClientSocket::~CClientSocket(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 CClientSocket::Init()
{
	NLbyte tempstring[NL_MAX_STRING_LENGTH];

	g_Log << "Opening client socket...\n";
	g_NetLog << "Opening client socket...\n";

	//m_Socket = nlOpen(0, m_ConnectionType);

	//nlGetLocalAddr(m_Socket, &m_Address);

	//g_NetLog << "Client address is " << nlAddrToString(&m_Address, tempstring) << ".\n";

	if (m_Socket == NL_INVALID)
	{
		g_Log.LOG_ERROR("Error opening socket!!\n");
		g_NetLog.LOG_ERROR("Error opening socket!!\n");

		return 0;
	}

	g_NetLog << "Opened client socket!!\n";

	//nlStringToAddr(m_ServersAddress.c_str(), &m_Address);

	//g_NetLog << "Address is " << nlAddrToString(&m_Address, tempstring) << ".\n";

	//if (!nlConnect(m_Socket, &m_Address))
	{
		//TODO: IF STOPPED HERE, FIGURE OUT MFC DLL, UNCOMMENT afxwin in stdafx
		//nlClose(m_Socket);
		
		g_Log.LOG_ERROR("Error connecting to socket!!\n");
		g_NetLog.LOG_ERROR("Error connecting to socket!!\n");

		return 0;
	}

	m_bInitialized = true;

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 CClientSocket::Update()
{
// 	static PlayerDataPacket playerUpdate;
// 	static PlayerData lastPlayerData;
	static Timer ErrorTimer(true);
	static UInt32 ErrorCount = 0;
#if 0
	if (IsInitialized())
	{
		if (g_Player)
		{
			m_PlayerData.Pos				= g_Player->m_vPosition;
			m_PlayerData.Dir				= g_Player->m_uDirection;
			m_PlayerData.State				= g_Player->m_eState;
			m_PlayerData.AnimState			= g_Player->m_eCurrentAnimState;

			if (memcmp(&lastPlayerData, &m_PlayerData, sizeof(PlayerData)) == 0)
			{
				lastPlayerData = m_PlayerData;

				playerUpdate.header.type		= PlayerUpdate;
				playerUpdate.pdData				= m_PlayerData;

				if (SendPacket(&playerUpdate) <= 0)
				{
					if (ErrorTimer >= 5 && ++ErrorCount <= 500)
					{
						PrintNLError;
						g_NetLog.LOG_ERROR("Failed to send player update!\n");
						ErrorTimer.Reset();
					}
				}
			}
		}

		if (!HandleIncomingData())
			return -1;

		ProcessQueue();
	}
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////