///////////////////////////////////////////////////////////////////////////
//		File:				SoundMgr.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A manager that loads and stores all sound effects
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\soundmgr.h"

#include "Globals.h"
#include "GameMacros.h"
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(SoundMgr);

static String  SoundsBasePath = "Res\\Sounds\\";

//////////////////////////////////////////////////////////////////////////

SoundMgr::SoundMgr(void)
{
	m_uNextID		= 0;
}

//////////////////////////////////////////////////////////////////////////

SoundMgr::~SoundMgr(void)
{
	UInt32		count	= m_Sounds.Size(),
			n		= 0;
	Sound	snd;

	DblLinkedList<Sound>::Iterator	i(m_Sounds);

	for (i.Begin(); !i.End(); ++i, n++)
	{
		snd = i.Current();

		g_ResLog << "Freeing sound " << snd.strFile << "...\n";

		SAFE_RELEASE(snd.dsBuffer);
	}

	g_Log << "Freed " << count << " sounds!\n";
	g_ResLog << "Freed " << count << " sounds!\n";

	m_Sounds.Clear();
}

//////////////////////////////////////////////////////////////////////////

BYTE* SoundMgr::LoadWaveFromFile(String FilePath, Sound* sound)
{
	FILE *file = NULL;

	HeaderChunk			hdr;
	ChunkHeader			chunk;

	char SndPath[MAX_PATH];

	strncpy_s(SndPath, SoundsBasePath.c_str(), MAX_PATH);
	strncat_s(SndPath, FilePath.c_str(), MAX_PATH);

	fopen_s(&file, SndPath, "rb");

	if (!file)
	{
		g_Log.LOG_ERROR("Couldn't open sound file " + FilePath + "!\n");
		return NULL;
	}

	fread(&hdr, sizeof(HeaderChunk), 1, file);

	if (memcmp(hdr.cRiffID, "RIFF", 4) || memcmp(hdr.cDataID, "WAVE", 4))
	{
		g_Log.LOG_ERROR("Invalid WAVE format!\t\"" + FilePath + "\"\n");
		fclose(file);
		return NULL;
	}

	fread(&chunk, sizeof(ChunkHeader), 1, file);

	UInt32 i;

	while (memcmp(chunk.cType, FORMAT_CHUNK, 4) && !feof(file))
	{
		for (i = 0; i < chunk.dwLength; i++)
		{
			UInt8 tmp;
			fread(&tmp, 1, 1, file);
		}

		fread(&chunk, sizeof(ChunkHeader), 1, file);
	}

	if (feof(file))
	{
		g_Log.LOG_ERROR("Error reading sound file!\n");
		fclose(file);
		return NULL;
	}

	fread(&sound->tFormat.wFormatTag,		sizeof(Int16),	1, file);
	fread(&sound->tFormat.nChannels,		sizeof(Int16),	1, file);
	fread(&sound->tFormat.nSamplesPerSec,	sizeof(UInt32),	1, file);
	fread(&sound->tFormat.nAvgBytesPerSec,	sizeof(UInt32),	1, file);
	fread(&sound->tFormat.nBlockAlign,		sizeof(Int16),	1, file);
	fread(&sound->tFormat.wBitsPerSample,	sizeof(Int16),	1, file);

	sound->tFormat.cbSize = 0;

	for (i = chunk.dwLength - 16; i > 0; i--)
	{
		UInt8 tmp;
		fread(&tmp, 1, 1, file);
	}

	fread(&chunk, sizeof(ChunkHeader), 1, file);

	while (memcmp(chunk.cType, "data", 4) && !feof(file))
	{
		for (i = 0; i < chunk.dwLength; i++)
		{
			UInt8 tmp;
			fread(&tmp, 1, 1, file);
		}

		fread(&chunk, sizeof(ChunkHeader), 1, file);
	}

	if (feof(file))
	{
		g_Log.LOG_ERROR("Error reading sound file!\n");
		fclose(file);
		return NULL;
	}

	//_set_new_mode(1);

	BYTE *buffer = (BYTE *) malloc(sizeof(BYTE) * chunk.dwLength);

	ASSERT(buffer);

	fread(buffer, sizeof(BYTE), chunk.dwLength, file);

	sound->nSampleSize = chunk.dwLength;

	fclose(file);

	return  buffer;
}

//////////////////////////////////////////////////////////////////////////

BYTE* SoundMgr::LoadWaveFromMemory(String FilePath, Sound* sound)
{
#ifdef LOAD_SOUNDS_FROM_PAK
	HeaderChunk			hdr;
	ChunkHeader			chunk;
	DPakFile			file;

	g_PakFile.GetFileByName(file, FilePath.c_str());

	ASSERT(file.IsValid());

	file.Read(&hdr, sizeof(HeaderChunk));

	if (memcmp(hdr.cRiffID, "RIFF", 4) || memcmp(hdr.cDataID, "WAVE", 4))
	{
		g_Log.LOG_ERROR("Invalid WAVE format!\t\"" + FilePath + "\"\n");
		return NULL;
	}

	file.Read(&chunk, sizeof(ChunkHeader));

	UInt32 i;

	while (memcmp(chunk.cType, FORMAT_CHUNK, 4) && !file.EoF())
	{
		for (i = 0; i < chunk.dwLength; i++)
		{
			UInt8 tmp;
			file.Read(&tmp, 1);
		}

		file.Read(&chunk, sizeof(ChunkHeader));
	}

	if (file.EoF())
	{
		g_Log.LOG_ERROR("Error reading sound file!\n");
		return NULL;
	}

	file.Read(&sound->tFormat.wFormatTag,		sizeof(Int16));
	file.Read(&sound->tFormat.nChannels,		sizeof(Int16));
	file.Read(&sound->tFormat.nSamplesPerSec,	sizeof(UInt32));
	file.Read(&sound->tFormat.nAvgBytesPerSec,	sizeof(UInt32));
	file.Read(&sound->tFormat.nBlockAlign,		sizeof(Int16));
	file.Read(&sound->tFormat.wBitsPerSample,	sizeof(Int16));

	sound->tFormat.cbSize = 0;

	for (i = chunk.dwLength - 16; i > 0; i--)
	{
		UInt8 tmp;
		file.Read(&tmp, 1);
	}

	file.Read(&chunk, sizeof(ChunkHeader));

	while (memcmp(chunk.cType, "data", 4) && !file.EoF())
	{
		for (i = 0; i < chunk.dwLength; i++)
		{
			UInt8 tmp;
			file.Read(&tmp, 1);
		}

		file.Read(&chunk, sizeof(ChunkHeader));
	}

	if (file.EoF())
	{
		g_Log.LOG_ERROR("Error reading sound file!\n");
		return NULL;
	}

	//_set_new_mode(1);

	BYTE *buffer = (BYTE *) malloc(sizeof(BYTE) * chunk.dwLength);

	ASSERT(buffer);

	file.Read(buffer, chunk.dwLength);

	sound->nSampleSize = chunk.dwLength;

	return  buffer;
#else
	return NULL;
#endif
}

//////////////////////////////////////////////////////////////////////////

Int32 SoundMgr::GetID(String FilePath)
{
	DblLinkedList<Sound>::Iterator i(m_Sounds);
	Int32 id = 0;

	for (i.Begin(); !i.End(); ++i, id++)
	{
		if (!strcmp(FilePath.c_str(), i.Current().strFile))
			return id;
	}

	//Shouldn't be here
	ASSERT(false);

	return -1;
}

//////////////////////////////////////////////////////////////////////////

Int32 SoundMgr::LoadSound(String FilePath)
{
	char filePath[MAX_PATH] = {'\0'};

	strncpy_s(filePath, FilePath.c_str(), MAX_PATH);
	strncat_s(filePath, ".wav", MAX_PATH);

	if (Contains(filePath))
	{
		//TODO: Convert Contains & GetID to ONE function that optionally returns ID if found, not true/false
		return GetID(filePath);
	}

	HRESULT hr;

	UInt8 *data = NULL;

	UInt8 *pAudio1		= NULL;
	UInt8 *pAudio2		= NULL;
	UInt32 audioLen1	= 0;
	UInt32 audioLen2	= 0;

	Sound sound;

#ifndef LOAD_SOUNDS_FROM_PAK
	data = LoadWaveFromFile(filePath, &sound);
#else
	data = LoadWaveFromMemory(filePath, &sound);
#endif

	ASSERT(data);

	g_ResLog << "Loading sound file " << filePath << "...\n";

	ZeroMemory(&sound.dsBufferDesc, sizeof(DSBUFFERDESC));
	sound.dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);

	sound.dsBufferDesc.dwFlags =	DSBCAPS_CTRLPAN 
								|	DSBCAPS_CTRLVOLUME 
								|	DSBCAPS_CTRLFREQUENCY 
								|	DSBCAPS_LOCDEFER 
								;

	sound.dsBufferDesc.dwBufferBytes = sound.nSampleSize;

	sound.dsBufferDesc.lpwfxFormat	= &sound.tFormat;

	if (FAILED(hr = g_Sound->GetSoundObject()->CreateSoundBuffer(&sound.dsBufferDesc, &sound.dsBuffer, NULL)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Couldn't create sound buffer!\n");

		free(data);

		return -1;
	}

	if (FAILED(hr = sound.dsBuffer->Lock(	0, sound.nSampleSize, 
											(void **) &pAudio1, LPDWORD(&audioLen1), 
											(void **) &pAudio2, LPDWORD(&audioLen2), 
											DSBLOCK_FROMWRITECURSOR)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Couldn't lock sound buffer!\n");

		free(data);

		return -1;
	}

	memcpy(pAudio1, data, audioLen1);
	memcpy(pAudio2, data + audioLen1, audioLen2);

	if (FAILED(hr = sound.dsBuffer->Unlock(pAudio1, audioLen1, pAudio2, audioLen2)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to unlock sound buffer!\n");

		free(data);

		return -1;
	}

	free(data);

	strncpy_s(sound.strFile, filePath, 128);

	m_Sounds.Add(sound);

	return m_uNextID++;
}

//////////////////////////////////////////////////////////////////////////

Bool SoundMgr::Contains(String FilePath)
{
	DblLinkedList<Sound>::Iterator	i(m_Sounds);

	for (i.Begin(); !i.End(); ++i)
	{
		if (!strcmp(i.Current().strFile, FilePath.c_str()))
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

Sound& SoundMgr::GetByID(UInt32 id)
{
	return m_Sounds[id];
}

//////////////////////////////////////////////////////////////////////////

void SoundMgr::PlaySound(UInt32 SoundId, Bool TimeScale, Bool Loop)
{
	HRESULT hr;
	DWORD status;

	if (SoundId == -1)
		return;

	Sound sound = m_Sounds[SoundId];

	if (FAILED(hr = sound.dsBuffer->GetStatus(&status)))
		g_Log.LOG_ERROR_DX_DESC(hr, "Error getting sound status!\n");

	if (status & DSBSTATUS_PLAYING || status & DSBSTATUS_LOOPING)
	{
		sound.dsBuffer->Stop();
		sound.dsBuffer->SetCurrentPosition(0);
	}

	DWORD freq;
	sound.dsBuffer->GetFrequency(&freq);

	if (TimeScale)
		sound.dsBuffer->SetFrequency(UInt32(freq * Timer::GetTimeScale()));
	else if (freq != sound.tFormat.nSamplesPerSec)
		sound.dsBuffer->SetFrequency(sound.tFormat.nSamplesPerSec);

	DWORD flags =	DSBPLAY_LOCHARDWARE
				|	DSBPLAY_TERMINATEBY_TIME
				;

	if (Loop)
		flags |= DSBPLAY_LOOPING;

	if (FAILED(hr = sound.dsBuffer->Play(0, 0, flags)))
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to play sound!\n");
}

//////////////////////////////////////////////////////////////////////////

void SoundMgr::StopSound(UInt32 SoundId)
{
	Sound sound = m_Sounds[SoundId];
	sound.dsBuffer->Stop();
	sound.dsBuffer->SetCurrentPosition(0);
}

//////////////////////////////////////////////////////////////////////////

void SoundMgr::SetSoundVolume(UInt32 SoundId, UInt32 volume)
{
	Sound sound = m_Sounds[SoundId];
	sound.dsBuffer->SetVolume(DecToPct(volume));
}

//////////////////////////////////////////////////////////////////////////