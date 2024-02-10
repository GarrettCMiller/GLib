///////////////////////////////////////////////////////////////////////////
//		File:				SoundMgr.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A manager that loads and stores all sound effects
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "DirectSound.h"

#include "LListDbl.h"
#include "Timer.h"

//////////////////////////////////////////////////////////////////////////

#define FORMAT_CHUNK		"fmt "
#define DATA_CHUNK			"data"

//////////////////////////////////////////////////////////////////////////

struct ChunkHeader
{
	UInt8	cType[4];
	UInt32	dwLength;
};

//////////////////////////////////////////////////////////////////////////

struct HeaderChunk
{
	UInt8	cRiffID[4];
	UInt32	dwDataLength;
	UInt8	cDataID[4];
};

//////////////////////////////////////////////////////////////////////////

struct Sound
{
	Sound()
	{
		ZeroMemory(strFile, 128);
		ZeroMemory(&tFormat, sizeof(WAVEFORMATEX));
		nSampleSize = 0;
		ZeroMemory(&dsBuffer, sizeof(LPDIRECTSOUNDBUFFER));
		ZeroMemory(&dsBufferDesc, sizeof(DSBUFFERDESC));
	}

	WAVEFORMATEX			tFormat;
	UInt32						nSampleSize;
	LPDIRECTSOUNDBUFFER		dsBuffer;
	DSBUFFERDESC			dsBufferDesc;
	char					strFile[128];
};

//////////////////////////////////////////////////////////////////////////

class SoundMgr : public Singleton<SoundMgr>
{
private:
	DblLinkedList<Sound>	m_Sounds;
	UInt32					m_uNextID;

private:
	BYTE*	LoadWaveFromFile(String FilePath, Sound* sound);
	BYTE*	LoadWaveFromMemory(String FilePath, Sound* sound);

	Bool	Contains(String FilePath);
	Int32		GetID(String FilePath);

public:
	SoundMgr(void);
	virtual ~SoundMgr(void);

	Int32		LoadSound(String FilePath);

	void	PlaySound(UInt32 SoundId, Bool TimeScale = (Timer::GetTimeScale() != 1.0f), Bool Loop = false);
	void	StopSound(UInt32 SoundId);
	void	SetSoundVolume(UInt32 SoundId, UInt32 volume);

	Sound&	GetByID(UInt32 id);
};

#define g_SoundMgr	SoundMgr::GetInstance()