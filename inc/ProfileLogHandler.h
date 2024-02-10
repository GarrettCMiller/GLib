///////////////////////////////////////////////////////////////////////////
//		File:				ProfileLogHandler.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A logger than spits out profiling info
//////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////
#include "Singleton.h"
#include "ProfilerOutputHandler.h"
#include "Logger.h"

//////////////////////////////////////////////////////////////////////////

class CProfileLogHandler :	public IProfilerOutputHandler, public Singleton<CProfileLogHandler>
{
	Logger	profileDump;

public:
	virtual ~CProfileLogHandler()
	{
		profileDump.~Logger();
	}

	void	BeginOutput(String FileName = "");
	void	EndOutput();
	void	Sample(Float fMin, Float fAvg, Float fMax, Int32 callCount, String name, Int32 parentCount);
};

#define g_ProfileHandler	CProfileLogHandler::GetInstance()