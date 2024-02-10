///////////////////////////////////////////////////////////////////////////
//		File:				Packets.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Defines packet types I use
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "MemMgr.h"
#ifdef _DOOMEROIDS_

#include "Entity.h"

//////////////////////////////////////////////////////////////////////////

struct PlayerData
{
	Int32						PlayerID;		//If < 0, it is a monster
	Vec2					Pos;
	UInt8					Dir;
	EEntityState			State;
	EEntityAnimationState	AnimState;
};

#endif
//////////////////////////////////////////////////////////////////////////

#define MAX_PACKET_DATA_COUNT		3
#define MAX_PACKET_STRING_SIZE		(MAX_PACKET_DATA_COUNT * 4)

//////////////////////////////////////////////////////////////////////////

enum PacketType
{
	Notify,

	PlayerJoin,
	PlayerQuit,
	PlayerUpdate,

	EntityCreate,
	EntityDestroy,
	EntityUpdate,

	EntityMessage,

	PacketTypeCount,

	InvalidPacketType = -1
};

//////////////////////////////////////////////////////////////////////////

//#pragma pack(1) // keep packet structs packed to original spec

struct PacketHeader
{
	UInt32					id;
	PacketType				type;
};

//////////////////////////////////////////////////////////////////////////

class Packet
{
public:
	PacketHeader	header;

public:
			Packet()	{	ZeroMemory(&header, sizeof(PacketHeader));	}
	virtual ~Packet()	{}

	void*	operator new(size_t size)
	{
		return g_MemMgr->AllocateMem(size, "Packet", 0, "Func");
	}

	void	operator delete(void* pMem)
	{
		g_MemMgr->Release(pMem);
	}
};

//////////////////////////////////////////////////////////////////////////

class PacketNotify : public Packet
{
public:
	PacketType	nextType;

public:
	PacketNotify() : Packet(), nextType(InvalidPacketType) {}
	~PacketNotify() { }

	void*	operator new(size_t size)
	{
		return g_MemMgr->AllocateMem(size, "PacketNotify", 0, "Func");
	}

	void	operator delete(void* pMem)
	{
		g_MemMgr->Release(pMem);
	}
};

//////////////////////////////////////////////////////////////////////////

class DataPacket : public Packet
{
public:
	union
	{
		Float	fData[MAX_PACKET_DATA_COUNT];
		UInt32	uData[MAX_PACKET_DATA_COUNT];
		Int32		nData[MAX_PACKET_DATA_COUNT];
		char	strData[MAX_PACKET_STRING_SIZE];
	};

public:
	DataPacket() : Packet()	{ ZeroMemory(fData, sizeof(Float) * MAX_PACKET_DATA_COUNT);	}
	~DataPacket() { }

	void*	operator new(size_t size)
	{
		return g_MemMgr->AllocateMem(size, "DataPacket", 0, "Func");
	}

	void	operator delete(void* pMem)
	{
		g_MemMgr->Release(pMem);
	}
};

//////////////////////////////////////////////////////////////////////////
#ifdef _DOOMEROIDS_

class PlayerDataPacket : public Packet
{
public:
	PlayerData	pdData;

public:
	PlayerDataPacket() : Packet()	{	ZeroMemory(&pdData, sizeof(PlayerData));	}
	~PlayerDataPacket() {}

	void*	operator new(size_t size)
	{
		return g_MemMgr->AllocateMem(size, "PlayerDataPacket", 0, "Func");
	}

	void	operator delete(void* pMem)
	{
		g_MemMgr->Release(pMem);
	}
};

//////////////////////////////////////////////////////////////////////////

class EntMsgPacket : public Packet
{
public:
	ENTITY_MESSAGE_ITEM		emData;

public:
	EntMsgPacket() : Packet()	{ ZeroMemory(&emData, sizeof(ENTITY_MESSAGE_ITEM)); }
	~EntMsgPacket() {}

	void*	operator new(size_t size)
	{
		return g_MemMgr->AllocateMem(size, "EntMsgPacket", 0, "Func");
	}

	void	operator delete(void* pMem)
	{
		g_MemMgr->Release(pMem);
	}
};

//#pragma pack() // restore default packing
//////////////////////////////////////////////////////////////////////////

#endif