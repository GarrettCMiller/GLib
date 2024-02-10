///////////////////////////////////////////////////////////////////////////
//		File:				MySocket.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A base socket class
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "nl.h"
#include "Packets.h"

#include "Queue.h"

//#include "Globals.h"
//////////////////////////////////////////////////////////////////////////

#define DEFAULT_CONNECTION_TYPE		NL_RELIABLE_PACKETS	

#define PrintNLError		g_NetLog <<		"NLError: "			<< nlGetErrorStr(nlGetError()) << "\n" <<\
											"NLSystemError:"	<< nlGetSystemErrorStr(nlGetSystemError()) << "\n"

//////////////////////////////////////////////////////////////////////////

class CMySocket
{
protected:
	NLsocket		m_Socket;
	NLaddress		m_Address;

	Bool			m_bInitialized;

#ifdef _DOOMEROIDS_
	PlayerData		m_PlayerData;
#endif

	Queue<Packet*>	m_Packets;
	Packet*			m_CurrentPacket;

	UInt32			m_TotalPacketsSent;
	UInt32			m_PacketsSent;
	UInt32			m_TotalPacketsReceived;
	UInt32			m_PacketsReceived;

	void			DumpNetStats()
	{
#if 0
		g_NetLog << "Total Packets Sent (includes notify): "		<< m_TotalPacketsSent <<
					"\nPackets Sent:\t\t "							<< m_PacketsSent <<
					"\nTotal Packets Received (includes notify): "	<< m_TotalPacketsReceived <<
					"\nPackets Received:\t\t "						<< m_PacketsReceived << '\n';
#endif
	}

	static const NLenum		m_ConnectionType = DEFAULT_CONNECTION_TYPE;

	static NLmutex	mutex;

	Int32			SendPacketCombo(Packet* packet, NLsocket socket);
	Int32			ReceivePacketCombo(Packet*& packet, NLsocket socket);

public:
	CMySocket(void);
	~CMySocket(void);

	virtual Int32	Init() = 0;
	virtual Int32 Update() = 0;

	virtual Int32 SendPacket(Packet* packet, NLsocket socket = 0);
	virtual Int32 ReceivePacket(Packet*& packet, NLsocket socket = 0);

#ifdef _DOOMEROIDS_
	virtual Int32	SendMessagePacket(EMI& entMsg);
#endif

	virtual Int32 HandleIncomingData();
	virtual Int32 ProcessQueue();

	Bool		IsInitialized()	const {return m_bInitialized;}
	
	//////////////////////////////////////////////////////////////////////////
#if 0
	virtual Int32 ProcessPacket();

	virtual Int32 ProcPlayerJoin();
	virtual Int32 ProcPlayerQuit();
	virtual Int32 ProcPlayerUpdate();

	virtual Int32 ProcEntityCreate();
	virtual Int32 ProcEntityDestroy();
	virtual Int32 ProcEntityUpdate();

	virtual Int32 ProcEntityMessage();
#endif
public:

	UInt32	BytesLost;
};
