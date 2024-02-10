/////////////////////////////////////////////////////////////////////////////
//		File:			DirectSound.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:52:07 AM
//
//		Description:	Wrapper class for DirectSound
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\directsound.h"

#include "MemMgr.h"
//#include "Game.h"

#include "GameMacros.h"

#include "Globals.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(DirectSound);

//////////////////////////////////////////////////////////////////////////

DirectSound::DirectSound(void)
{
	m_lpDSObj		= NULL;
	m_lpMainBuffer	= NULL;
	m_fMasterVolume	= 0.8f;
}

//////////////////////////////////////////////////////////////////////////

DirectSound::~DirectSound(void)
{
	if (!Shutdown())
	{
		g_Log.LOG_ERROR("Failed to shut down DirectSound properly!\n");
	}
}

//////////////////////////////////////////////////////////////////////////

Int32 DirectSound::Init(Bool Priority)
{
	HRESULT hr = S_OK;

	g_Log << "DirectSound::Init() begin...\n";

	if (FAILED(hr = DirectSoundCreate8(NULL, &m_lpDSObj, NULL)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to create DirectSound!\n");
		return 0;
	}

	if (FAILED(hr = m_lpDSObj->SetCooperativeLevel(g_hWnd, (Priority ? DSSCL_PRIORITY : DSSCL_NORMAL))))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to set cooperative level for DirectSound!\n");
		return 0;
	}

	if (Priority != NULL)
	{

		if (!CreateMainBuffer())
		{
			g_Log.LOG_ERROR("Problem creating main sound buffer!\n");
			return 0;
		}
	}

	g_Log << "DirectSound::Init() end!\n";

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Bool DirectSound::CreateMainBuffer()
{
	g_Log << "Creating main sound buffer with priority...\n";

	HRESULT hr;

	ZeroMemory(&m_wfxMainBufferFmt,		sizeof(WAVEFORMATEX));
		
	m_wfxMainBufferFmt.wFormatTag		= WAVE_FORMAT_PCM;
	m_wfxMainBufferFmt.nChannels		= 2;
	m_wfxMainBufferFmt.wBitsPerSample	= 16;
	m_wfxMainBufferFmt.nSamplesPerSec	= 44100;

	m_wfxMainBufferFmt.nBlockAlign		= m_wfxMainBufferFmt.nChannels * (m_wfxMainBufferFmt.wBitsPerSample / 8);
	m_wfxMainBufferFmt.nAvgBytesPerSec	= m_wfxMainBufferFmt.nSamplesPerSec * m_wfxMainBufferFmt.nBlockAlign;

	ZeroMemory(&m_dsMainBufferDesc,		sizeof(DSBUFFERDESC));
	
	m_dsMainBufferDesc.dwSize			= sizeof(DSBUFFERDESC);
	m_dsMainBufferDesc.dwFlags			=	DSBCAPS_PRIMARYBUFFER
										|	DSBCAPS_CTRLVOLUME
										|	DSBCAPS_CTRLPAN
										;

	if (FAILED(hr = m_lpDSObj->CreateSoundBuffer(&m_dsMainBufferDesc, &m_lpMainBuffer, NULL)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Error creating main sound buffer!\n");
		return false;
	}

	if (FAILED(hr = m_lpMainBuffer->SetFormat(&m_wfxMainBufferFmt)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Error setting main buffer format!\n");
		return false;
	}

	//dsBuffer->SetVolume(DecToPct(volume));
	//hr = m_lpMainBuffer->SetVolume(DecToPct(25));
	//hr = m_lpMainBuffer->SetVolume(5000);
	//hr = m_lpMainBuffer->SetPan(-75000);
	//g_Log.LOG_ERROR_DX(hr);

	g_Log << "Created main sound buffer!\n";

	return true;
}

//////////////////////////////////////////////////////////////////////////

Int32 DirectSound::Shutdown()
{
	Bool bError = false;

	if (m_lpMainBuffer)
	{
		if (!BAD_PTR(m_lpMainBuffer))
		{
			m_lpMainBuffer->Release();
			m_lpMainBuffer = NULL;

			g_Log << "Released DirectSound buffer!\n";
		}
		else
		{
			g_Log.LOG_ERROR("BAD POINTER TO MAIN SOUND BUFFER!\n");
			bError = true;
			ASSERT(false);
		}
	}

	if (m_lpDSObj)
	{
		m_lpDSObj->Release();
		m_lpDSObj = NULL;

		g_Log << "Released DirectSound object!\n";
	}

	return Int32(!bError);
}

//////////////////////////////////////////////////////////////////////////