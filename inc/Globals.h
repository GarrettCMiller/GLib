/////////////////////////////////////////////////////////////////////////////
//		File:			Globals.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:50:12 AM
//
//		Description:		
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Logger.h"

///////////////////////////////////////////////////////////
//DEFINED VALUES
///////////////////////////////////////////////////////////
#define	DEF_WINDOW_WIDTH		1280
#define	DEF_WINDOW_HEIGHT		1024

//One kilobyte (KB) is 1024 bytes
#define ONE_KILOBYTE			1024
//One megabyte (MB) is 1024 KB
#define ONE_MEGABYTE			1024 * ONE_KILOBYTE

//Memory pool size of 5 MB (1024 bytes * 1024 bytes * 5)
#define	DEF_POOL_SIZE			5 * ONE_MEGABYTE

#define MAX_PLAYERS				5

#define	ERROR_BUFFER_SIZE		512

//////////////////////////////////////////////////////////////////////////

#define MAX_FILENAME_LENGTH		128
#define MAX_FILEPATH_LENGTH		128

#define MAX_CLIENTS				10

//////////////////////////////////////////////////////////////////////////

#define	RangedVariable(_Type, _name)	_Type _name; _Type _name##Max; _Type _name##Min;
#define SteppedVariable(_Type, _name)	_Type _name; _Type _name##Max; _Type _name##Min; _Type _name##Step;

#define FloatToInt(_val)				(INT_MAX * _val)

//For using int based slider controls with float vars
#define SetFloatRange(min, max)		SetRange(min * g_fFloatPrecision, max * g_fFloatPrecision, true)
#define SetFloatPos(pos)			SetPos(pos * g_fFloatPrecision)


//////////////////////////////////////////////////////////////////////////

#include "dpakfile.h"

//////////////////////////////////////////////////////////////////////////

//Handle to the window
extern HWND						g_hWnd;
extern HINSTANCE				g_hInst;
extern HRESULT					g_hr;

//Window is active or not
extern bool						g_bActive;

//Main logger
extern Logger					g_Log;
//Resource logger
extern Logger					g_ResLog;
//Network logger
extern Logger					g_NetLog;

//Default run speed for all creatures
//extern Vec2						gk_RunSpeed;

extern CDPakFile				g_PakFile;

//For using int based slider controls with float vars
extern const float				g_fFloatPrecision;
extern const float				g_fInvFloatPrecision;

extern char						g_ErrorBuffer[ERROR_BUFFER_SIZE];

extern const char				g_strLetters[];
extern const char				g_strUpperLetters[];
extern const char				g_strLowerLetters[];


//////////////////////////////////////////////////////////////////////////