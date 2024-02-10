// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0500		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0501 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0601	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some String constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <afxwin.h>         // MFC core and standard components

// C RunTime Header Files
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <crtdbg.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
//#include <string>

#ifdef _DEBUG
	#define D3D_DEBUG_INFO
#endif

#define _GLIB_DLL_API_

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

#define __WFILE__		WIDEN(__FILE__)
#define __WFUNCTION__	WIDEN(__FUNCTION__)

//Disable non-standard extension (Logger::Error to Logger::Error&)
#pragma warning (disable: 4239)
//Disable non-standard extension (type name first seen using 'class' now seen using 'struct')
#pragma warning (disable: 4099)

//#pragma warning (disable: )

#include "DataTypes.h"
#include "Globals.h"

#include "Logger.h"

//Main logger
extern Logger					g_Log;
//Resource logger
extern Logger					g_ResLog;
//Network logger
extern Logger					g_NetLog;