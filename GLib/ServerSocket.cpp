///////////////////////////////////////////////////////////////////////////
//		File:				ServerSocket.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Implementation of a server socket
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\serversocket.h"
#include "ClientSocket.h"
//#include "MapMgr.h"
#include "SpriteMgr.h"

#include "Globals.h"

//////////////////////////////////////////////////////////////////////////

CServerSocket::CServerSocket(void) : 
	m_NumClients(0)
{
	ZeroMemory(m_Client, sizeof(NLsocket) * MAX_PLAYERS);
	
	m_ErrorTimer.Reset();
}

//////////////////////////////////////////////////////////////////////////

CServerSocket::~CServerSocket(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 CServerSocket::Init()
{
	NLbyte tempstring[NL_MAX_STRING_LENGTH];

	g_Log << "Opening server socket...\n";
	g_NetLog << "Opening server socket...\n";

	//m_Socket = nlOpen(25000, m_ConnectionType);

	if (m_Socket == NL_INVALID)
	{
		g_Log.LOG_ERROR("Error opening socket!!\n");
		g_NetLog.LOG_ERROR("Error opening socket!!\n");

		return 0;
	}

	g_NetLog << "Opened server socket!!\n";

	/*if (!nlListen(m_Socket))
	{
		nlClose(m_Socket);

		g_Log.LOG_ERROR("Error listening on socket!!\n");
		g_NetLog.LOG_ERROR("Error listening on socket!!\n");
	}*/

	g_NetLog << "Listening for connections...\n";

	//nlGetLocalAddr(m_Socket, &m_Address);

	//g_NetLog << "Server address is " << nlAddrToString(&m_Address, tempstring) << ".\n";

	m_bInitialized = true;

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 CServerSocket::Update()
{
	if (IsInitialized())
	{
		CheckForNewClients();

 		Packet* packet = new Packet;
 
 		for (Int32 i = 0; i < m_NumClients; i++)
 		{
 			if (ReceivePacket(packet, m_Client[i]) > 0)
 			{
 				for (Int32 j = 0; j < m_NumClients; j++)
 				{
 					if (i != j)
 						SendPacket(packet, m_Client[j]);
 				}
 			}
 		}

		delete packet;

		return 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CServerSocket::CheckForNewClients()
{
	NLbyte		tempstring[NL_MAX_STRING_LENGTH];

	NLsocket	newsock = NULL;//nlAcceptConnection(m_Socket);

	if (newsock != NL_INVALID)
	{
		NLaddress addr;

		//nlGetRemoteAddr(newsock, &addr);
		m_Client[m_NumClients] = newsock;

		//nlGroupAddSocket(m_Group, newsock);

		//g_NetLog << "Client " << m_NumClients << " connected from " << nlAddrToString(&addr, tempstring) << ".\n";
		//_cprintf("Client %d connected from %s.\n", m_NumClients, tempstring);

		DataPacket	newPlayer;

		newPlayer.header.type		= PlayerJoin;
		newPlayer.uData[0]			= m_NumClients;

		for (Int32 i = 0; i <= m_NumClients; i++)
		{
			if (SendPacket(&newPlayer, m_Client[i]) <= 0)
			{
				//PrintNLError;
				_cprintf("Error sending PlayerJoin message!\n");
				g_NetLog.LOG_ERROR("Error sending PlayerJoin message!\n");
				return -1;
			}
			else
			{
				_cprintf("Sent client info to client %d!\n", i);
				g_NetLog << "Sent client info to client " << i << "!\n";
			}

			if (i == m_NumClients)
			{
				for (Int32 j = 0; j < m_NumClients; j++)
				{
					newPlayer.uData[0] = j;
					if (SendPacket(&newPlayer, m_Client[i]) <= 0)
					{
						//PrintNLError;
						_cprintf("Couldn't send player info to new player!\n");
						g_NetLog.LOG_ERROR("Couldn't send player info to new player!\n");
						return -1;
					}
					else
					{
						_cprintf("Sent player %d to new player!\n", j);
						g_NetLog << "Sent player " << j << " to new player!\n";
					}
				}
			}
		}			

		m_NumClients++;
	}
	else
	{
		/*if (nlGetError() == NL_SYSTEM_ERROR)
		{
			g_Log.LOG_ERROR("System error in HawkNL!!\n");
			g_NetLog.LOG_ERROR("System error in HawkNL!!\n");

			return -1;
		}*/
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////