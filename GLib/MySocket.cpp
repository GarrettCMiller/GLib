///////////////////////////////////////////////////////////////////////////
//		File:				MySocket.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Base socket class
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\mysocket.h"

//#include "MapMgr.h"
#include "DPlay.h"

#include "Globals.h"

//////////////////////////////////////////////////////////////////////////

CMySocket::CMySocket(void) :
	m_Socket(0),
	m_bInitialized(false),
	m_CurrentPacket(NULL),
	BytesLost(0),
	m_TotalPacketsSent(0),
	m_PacketsSent(0),
	m_TotalPacketsReceived(0),
	m_PacketsReceived(0)
{
	ZeroMemory(&m_Address,		sizeof(NLaddress));
//	ZeroMemory(&m_PlayerData,	sizeof(PlayerData));

	//m_CurrentPacket = new Packet;
}

//////////////////////////////////////////////////////////////////////////

CMySocket::~CMySocket(void)
{
	//TODO: UNCOMMENT AND FIX
	//if (m_bInitialized)
		//nlClose(m_Socket);

	//if (m_CurrentPacket)
	//	delete m_CurrentPacket;

	Int32 count = m_Packets.Size();
	Packet* p = NULL;

	for (Int32 i = 0; i < count; i++)
	{
		m_Packets.Dequeue(p);

		if (p)
			delete p;
	}

	m_Packets.Clear();
}

//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::SendPacketCombo(Packet* packet, NLsocket socket)
{
	ASSERT(packet);

	if (socket == 0)
		socket = m_Socket;

	PacketNotify pnotify;

	pnotify.header.type = Notify;
	pnotify.header.id = 0xffffffff;
	pnotify.nextType = packet->header.type;

	UInt32 packetsize = 0;
	Int32 ret = -1;
	
	DataPacket*			dp		= NULL;
	//PlayerDataPacket*	pdp		= NULL;
	//EntMsgPacket*		emp		= NULL;

	//Make sure not to send the vtable, so move the pointer 4 bytes and send 4 bytes less
	/*if ((ret = nlWrite(socket, ((BYTE *)&pnotify) + sizeof(void *), sizeof(PacketNotify) - sizeof(void *))) <= 0)
	{
		if (ret == 0)
		{
			DumpNetStats();
			g_NetLog.LOG_ERROR("Failed to send notify packet! Network buffer full!\n");
		}
		else
		{
			g_NetLog.LOG_ERROR("Failed to send notify packet!\n");
			PrintNLError;
		}
		return ret;
	}*/

	m_TotalPacketsSent++;

	switch (pnotify.nextType)
	{
	case PlayerJoin:
	case PlayerQuit:
		packetsize = sizeof(DataPacket) - sizeof(void *);
		dp = static_cast<DataPacket*>(packet);

		//if ((ret = nlWrite(socket, ((BYTE *) dp) + sizeof(void *), packetsize)) <= 0)
		//{
		//	if (ret == 0)
		//	{
		//		DumpNetStats();
		//		g_NetLog.LOG_ERROR("Buffers are full!");
		//	}

		//	g_NetLog.LOG_ERROR("Couldn't send data packet!\n");
		//}
		//else
		//{
		//	m_TotalPacketsSent++;
		//	m_PacketsSent++;
		//}
		break;

	case PlayerUpdate:
		//packetsize = sizeof(PlayerDataPacket) - sizeof(void *);
		//pdp = static_cast<PlayerDataPacket*>(packet);

		if (0)//(ret = nlWrite(socket, ((BYTE *) pdp) + sizeof(void *), packetsize)) <= 0)
		{
			if (ret == 0)
			{
				DumpNetStats();
				g_NetLog.LOG_ERROR("Buffers are full!");
			}

			g_NetLog.LOG_ERROR("Couldn't send player data packet!\n");
		}
		else
		{
			m_TotalPacketsSent++;
			m_PacketsSent++;
		}
		break;

	case EntityCreate:
	case EntityDestroy:
	case EntityUpdate:
		ASSERT(false);
		break;

	case EntityMessage:
		//packetsize = sizeof(EntMsgPacket) - sizeof(void *);
		//emp = static_cast<EntMsgPacket*>(packet);

		if (0)//(ret = nlWrite(socket, ((BYTE *) emp) + sizeof(void *), packetsize)) <= 0)
		{
			if (ret == 0)
			{
				DumpNetStats();
				g_NetLog.LOG_ERROR("Buffers are full!");
			}

			g_NetLog.LOG_ERROR("Couldn't send entity message packet!\n");
		}
		else
		{
			m_TotalPacketsSent++;
			m_PacketsSent++;
		}
	}	

	return ret;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::SendPacket(Packet* packet, NLsocket socket)
{
	return SendPacketCombo(packet, socket);
}

//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::ReceivePacketCombo(Packet*& /*packet*/, NLsocket socket)
{
	if (socket == 0)
		socket = m_Socket;

	PacketNotify pnotify;

	UInt32 packetsize = sizeof(PacketNotify) - sizeof(void *);
	Int32 ret = -1;
	Int32 tries = 0;

reread:
	//Make sure not to overwrite the vtable, so move the pointer 4 bytes and read 4 bytes less
	/*if ((ret = nlRead(socket, ((BYTE *)&pnotify) + sizeof(void *), packetsize)) <= 0)
	{
		if (ret < 0)
		{
			if (nlGetError() == NL_BUFFER_SIZE && ++tries < 10)
			{
				packetsize = sizeof(PacketNotify);
				g_NetLog.LOG_ERROR("Trying bigger size packet!\n");
				goto reread;
			}

			g_NetLog.LOG_ERROR("Failed to read notify packet!\n");
			PrintNLError;

			if (nlGetError() == NL_CON_REFUSED)
			{
				if (!Init())
				{
					g_Log.LOG_ERROR("Couldn't reinit!\n");
					g_NetLog.LOG_ERROR("Couldn't reinit!\n");
				}
			}
		}

		return ret;
	}*/

	packetsize = 0;

	m_TotalPacketsReceived++;
#if 0
	DataPacket*			dp		= NULL;
	PlayerDataPacket*	pdp		= NULL;
	EntMsgPacket*		emp		= NULL;

	switch (pnotify.nextType)
	{
	case PlayerJoin:
	case PlayerQuit:
		packetsize = sizeof(DataPacket) - sizeof(void *);
		dp = new DataPacket;
		
		if ((ret = nlRead(socket, ((BYTE *) dp) + sizeof(void *), packetsize)) <= 0)
		{
			DumpNetStats();
			g_NetLog.LOG_ERROR("No data packet!!\n");
		}
		else
		{
			packet = dp;
			m_TotalPacketsReceived++;
			m_PacketsReceived++;
		}
		break;

	case PlayerUpdate:
		packetsize = sizeof(PlayerDataPacket) - sizeof(void *);
		pdp = new PlayerDataPacket;

		if ((ret = nlRead(socket, ((BYTE *) pdp) + sizeof(void *), packetsize)) <= 0)
		{
			if (ret < 0)
				PrintNLError;

			DumpNetStats();

			g_NetLog.LOG_ERROR("No player data packet!!\n");
		}
		else
		{
			packet = pdp;
			m_TotalPacketsReceived++;
			m_PacketsReceived++;
		}
		break;

	case EntityCreate:
	case EntityDestroy:
	case EntityUpdate:
		ASSERT(false);
		break;

	case EntityMessage:
		packetsize = sizeof(EntMsgPacket) - sizeof(void *);
		emp = new EntMsgPacket;

		if ((ret = nlRead(socket, ((BYTE *) emp) + sizeof(void *), packetsize)) <= 0)
		{
			DumpNetStats();
			g_NetLog.LOG_ERROR("No entity message packet!!\n");
		}
		else
		{
			packet = emp;
			m_TotalPacketsReceived++;
			m_PacketsReceived++;
		}
	}
#endif
	return ret;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::ReceivePacket(Packet*& packet, NLsocket socket)
{
	return ReceivePacketCombo(packet, socket);
}

//////////////////////////////////////////////////////////////////////////
#if 0
Int32 CMySocket::SendMessagePacket(EMI& entMsg)
{
	EntMsgPacket packet;

	packet.emData		= entMsg;
	packet.header.type	= EntityMessage;

	if (SendPacket(&packet) < 0)
	{
		PrintNLError;
		return -1;
	}

	return 1;
}
#endif
//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::HandleIncomingData()
{
	Packet*	packet = NULL;

	while (ReceivePacket(packet) > 0)
	{
		if (packet)
			m_Packets.Enqueue(packet);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::ProcessQueue()
{
	m_CurrentPacket = NULL;

	while (!m_Packets.IsEmpty())
	{
		m_Packets.Dequeue(m_CurrentPacket);

		//ProcessPacket();

		delete m_CurrentPacket;
		m_CurrentPacket = NULL;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
#if 0
Int32 CMySocket::ProcessPacket()
{
	switch (m_CurrentPacket->header.type)
	{
	case PlayerJoin:
		return ProcPlayerJoin();

	case PlayerQuit:
		return ProcPlayerQuit();

	case PlayerUpdate:
		return ProcPlayerUpdate();

	case EntityCreate:
		return ProcEntityCreate();

	case EntityDestroy:
		return ProcEntityDestroy();

	case EntityUpdate:
		return ProcEntityUpdate();

	case EntityMessage:
		return ProcEntityMessage();

	default:
		return -1;
	}
}

//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::ProcPlayerJoin()
{
	DataPacket* packet = static_cast<DataPacket*>(m_CurrentPacket);

	Bool bLocal = false;
	UInt32 playerID = packet->uData[0];

	if (m_PlayerData.PlayerID == -1)
	{
		bLocal = true;

		m_PlayerData.PlayerID = playerID;

		g_NetLog << "Joined as player " << playerID << "!\n";
		_cprintf("Got client info!\nJoined as player %d!\n", playerID);	
	}
	else
	{
		g_NetLog << "Initializing remote player " << playerID << "...\n";
		_cprintf("Initializing remote player %d...\n", playerID);
	}

	return g_MapMgr->InitPlayer(bLocal, playerID);
}

//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::ProcPlayerQuit()
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::ProcPlayerUpdate()
{
	PlayerData inPlayerData = static_cast<PlayerDataPacket*>(m_CurrentPacket)->pdData;

	g_DPlay->m_PlayerList[inPlayerData.PlayerID]->m_vPosition			= inPlayerData.Pos;
	g_DPlay->m_PlayerList[inPlayerData.PlayerID]->m_uDirection			= inPlayerData.Dir;
	g_DPlay->m_PlayerList[inPlayerData.PlayerID]->m_eState				= inPlayerData.State;
	g_DPlay->m_PlayerList[inPlayerData.PlayerID]->m_eCurrentAnimState	= inPlayerData.AnimState;

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::ProcEntityCreate()
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::ProcEntityDestroy()
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::ProcEntityUpdate()
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMySocket::ProcEntityMessage()
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////
#endif