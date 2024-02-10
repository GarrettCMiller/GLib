/////////////////////////////////////////////////////////////////////////////
//		File:					Globals.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:50:21 AM
//
//		Description:		
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "Globals.h"
//#include "Timer.h"

Float	Timer::ms_TimeScale	= 1.0f;
Timer	Timer::ms_GlobalTimer;

HWND			g_hWnd;
HINSTANCE		g_hInst;
HRESULT			g_hr;

Bool			g_bActive;

Logger			g_Log(true);
Logger			g_ResLog;
Logger			g_NetLog;

//Vec2			gk_RunSpeed = Vec2(1.0f, 1.0f);

const float		g_fFloatPrecision = 10000.0f;
const float		g_fInvFloatPrecision = 1 / g_fFloatPrecision;

char			g_ErrorBuffer[ERROR_BUFFER_SIZE];

const char		g_strLetters[] = "ABCDEFGHIJGKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char		g_strUpperLetters[] = "ABCDEFGHIJGKLMNOPQRSTUVWXYZ";
const char		g_strLowerLetters[] = "abcdefghijklmnopqrstuvwxyz";

CDPakFile		g_PakFile;

//////////////////////////////////////////////////////////////////////////