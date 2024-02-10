///////////////////////////////////////////////////////////////////////////
//		File:				XMLTables.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Database-like tables that define entities and stuff
//////////////////////////////////////////////////////////////////////////
#pragma once

// #ifndef _STDAFX_INCLUDED_
// 	#include "stdafx.h"
// 	#pragma message("stdafx.h has been included in " __FILE__)
// #endif

//#include "Globals.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class XMLSettings
{
public:
	struct Video
	{
		struct Resolution
		{
			Resolution(Int32 _width = 1024, Int32 _height = 768, Bool _windowed = true)
				:	width(_width), height(_height), windowed(_windowed)
			{			}

			Int32	width;
			Int32	height;
			Bool	windowed;
		};

		struct Advanced
		{
			Advanced(Bool _enableShaders = true) 
				: enableShaders(_enableShaders)
			{		}

			Bool	enableShaders;
		};

		Resolution	resolution;
		Advanced	advanced;
	};

	/////////////////////////////////////////////////////////////////////////

	struct Performance
	{
		Performance(Bool _throttleFPS = true, Float _throttleLimit = 60.0f /* fps */)
			: throttleFPS(_throttleFPS), throttleLimit(_throttleLimit)
		{ }

		Bool	throttleFPS;
		Float	throttleLimit;
	};

	/////////////////////////////////////////////////////////////////////////

	struct Memory
	{
		Memory(Int32 _reserve = 5 * 1024 * 1024)
			: reserve(_reserve)
		{		}

		Int32		reserve;
	};

	/////////////////////////////////////////////////////////////////////////

	struct Debug
	{
		Debug(Bool _enabled = true, Bool _showFPS = false)
			: enabled(_enabled), showFPS(_showFPS)
		{		}

		struct MapInfo
		{
			MapInfo(Bool _render = true, Bool _autospawn = true)
				: render(_render), autospawn(_autospawn)
			{		}

			Bool render;
			Bool autospawn;
		};

		struct Profiler
		{
			Profiler(Bool _enabled = true)
				: enabled(_enabled)
			{		}

			Bool	enabled;
		};

		Bool		enabled;
		Bool		showFPS;
		MapInfo		mapInfo;
		Profiler	profiler;
	};

	/////////////////////////////////////////////////////////////////////////

	Video		videoRes;
	Performance	performance;
	Memory		memory;
	Debug		debug;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

extern XMLSettings	g_Settings;


