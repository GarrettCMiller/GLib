/////////////////////////////////////////////////////////////////////////////
//		File:			DirectSound.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:52:05 AM
//
//		Description:	Wrapper class for DirectSound
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

//#include "stdafx.h"
#include "mmreg.h"

#include <dsound.h>
#include "dxerr.h"

#include "Singleton.h"
//////////////////////////////////////////////////////////////////////////

#pragma comment (lib, "dsound")
#pragma comment (lib, "dxguid")
#pragma comment (lib, "d3dx9")
#pragma comment (lib, "dxerr")

//////////////////////////////////////////////////////////////////////////

#define DecToPct(vol)		(-100 * (100 - vol))

////////////////////////////////////////////////////////////////////

class DirectSound : public Singleton<DirectSound>
{
	LPDIRECTSOUND8			m_lpDSObj;
	
	LPDIRECTSOUNDBUFFER		m_lpMainBuffer;
	WAVEFORMATEX			m_wfxMainBufferFmt;
	DSBUFFERDESC			m_dsMainBufferDesc;

	Float					m_fMasterVolume;

	Bool					CreateMainBuffer();

public:
	DirectSound(void);
	virtual ~DirectSound(void);

	Int32		Init(Bool Priorty = true);
	Int32		Shutdown();

	LPDIRECTSOUND8&	GetSoundObject()	{ return m_lpDSObj; }
};

#define g_Sound		DirectSound::GetInstance()