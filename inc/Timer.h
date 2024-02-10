/////////////////////////////////////////////////////////////////////////////
//		File:			Timer.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:49:20 AM
//
//		Description:	A precision timer
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "windows.h"
#include <time.h>

#include "DataTypes.h"
//////////////////////////////////////////////////////////////////////////

#define COMP(op)		Bool operator op (Float time) const \
						{	return (Get() op time);	}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

class Timer
{
	static Float	ms_TimeScale;

	Int64 	iFreq;
	Int64 	TimerLast;
	Int64	Elapsed;
	Bool	bUseTimeScale;
	Bool	bFrozen;

public:
	static Timer	ms_GlobalTimer;

public:
	Timer(Bool StartNow = false, Bool bTimeScale = true) : bUseTimeScale(bTimeScale)
	{
		Elapsed = 0;
		bFrozen = false;

		ZeroMemory(&iFreq,		sizeof(Int64));
		ZeroMemory(&TimerLast,	sizeof(Int64));

		QueryPerformanceFrequency((LARGE_INTEGER*)&iFreq);

		if (StartNow)
			Init();
	}

	void	Init()		{ QueryPerformanceCounter((LARGE_INTEGER*)&TimerLast); }
	void	Reset()		{ QueryPerformanceCounter((LARGE_INTEGER*)&TimerLast); }

	Bool	IsFrozen() const	{ return bFrozen;	}
	void	Freeze()
	{
		if (IsFrozen()) return;

		Elapsed += GetLarge();
		bFrozen = true;
	}

	void Thaw()
	{
		if (IsFrozen())
		{
			bFrozen = false;

		}
	}

	//Time comparison
	COMP(>)
	COMP(>=)
	COMP(<)
	COMP(<=)
	COMP(==)
	COMP(!=)

	Int64 GetLarge() const
	{
		static Int64 Current;
		
		if (IsFrozen())
		{
			Current = Elapsed;
		}
		else
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&Current);
			Current -= TimerLast;
			Current += Elapsed;
		}

		return	Current;
	}
	
	virtual Float Get() const
	{
		static Int64 Current;

		if (IsFrozen())
		{
			Current = Elapsed;
		}
		else
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&Current);
			Current -= TimerLast;
			Current += Elapsed;
		}

		return	(static_cast<Float>(Current) / (static_cast<Float>(iFreq))) * (bUseTimeScale ? ms_TimeScale : 1.0f);
	}

	static void SetTimeScale(Float timeScale)	
	{ 
		ASSERT(timeScale >= 0.0f);
		ms_TimeScale = timeScale;
	}

	static Float GetTimeScale()
	{
		return ms_TimeScale;
	}

	operator Float() const
	{
		return Get();
	}
};

#undef COMP
//class Profiler : public Timer
//{
//	Float Time;
//
//public:
//	Profiler() : Timer(false, false) 
//	{ }
//
//	void Begin()	{ Reset();	}
//
//	void End(char *FunctionName = NULL)
//	{
//		Time = Timer::Get();
//
//		//if (FunctionName)
//		//	g_Log << FunctionName << " took " << Time << " seconds (" << Time * 1000.0f << "ms).\n";
//	}
//	
//	virtual Float	Get()	{ return Time; }
//};