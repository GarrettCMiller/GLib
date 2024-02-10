///////////////////////////////////////////////////////////////////////////
//		File:				ProfilerOutputHandler.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		The base interface for a profiler output handler
//////////////////////////////////////////////////////////////////////////
#pragma once
//#include "Globals.h"
//////////////////////////////////////////////////////////////////////////

class IProfilerOutputHandler
{
public:
	virtual void BeginOutput(String FileName)		= 0;
	virtual void EndOutput()						= 0;

	virtual void Sample(Float fMin, Float fAvg, Float fMax, 
						Int32 callCount, String name, Int32 parentCount) = 0;
};
