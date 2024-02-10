///////////////////////////////////////////////////////////////////////////
//		File:				ProfileSample.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Defines a profiler
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "ProfileLogHandler.h"

//////////////////////////////////////////////////////////////////////////

#define MAX_PROFILER_SAMPLES	128

//#define USE_PROFILING

//////////////////////////////////////////////////////////////////////////

class CProfileSample
{
public:
	CProfileSample(const char* name);
	virtual ~CProfileSample(void);

	void Create(const char* name);

	void Start();
	void Stop();

	static void Output(String FileName = "");

	static void ResetSample(String name);
	static void ResetAll();

	static IProfilerOutputHandler*	m_OutputHandler;
	
	static Bool						ms_bProfilerEnabled;

	static struct ProfileSample
	{
		ProfileSample()
		{
			bIsValid = false;
			DataCount = 0;
			avgPc = minPc = maxPc = -1;
			nID = -1;
		}

		Bool			bIsValid;
		Bool			bIsOpen;
		UInt32			CallCount;
		String			Name;

		Float			fStartTime;
		Float			fTotalTime;
		Float			fChildTime;

		Float			avgPc;
		Float			minPc;
		Float			maxPc;

		Int32				nID;

		Int32				ParentCount;

		UInt32			DataCount;
	};

	static ProfileSample	m_Samples[MAX_PROFILER_SAMPLES];
	static ProfileSample internalSamples[10];

protected:
	Int32		m_SampleIndex;
	Int32		m_ParentIndex;

	inline Float GetTime();

	static Int32 ms_LastOpenedSample;
	static Int32 ms_OpenSampleCount;

	static Int32 ms_UniqueID;
	Int32			m_UniqueID;

	static Float ms_RootBegin, ms_RootEnd;
};

//////////////////////////////////////////////////////////////////////////

struct ProfileHelper
{
	ProfileHelper(CProfileSample* pProf)
	{
		m_pProf = pProf;
		m_pProf->Start();
	}

	~ProfileHelper()
	{
		m_pProf->Stop();
	}

	CProfileSample* m_pProf;
};

//////////////////////////////////////////////////////////////////////////

#ifdef USE_PROFILING
	#define PROFILE(name)	static CProfileSample pSamp(name); ProfileHelper pfHelper(&pSamp);
	#define PROFILE_THIS	PROFILE(__FUNCTION__)
	#define PROFILE_NO_HELPER(name)	static CProfileSample pSamp(name);
#else
	#define PROFILE(name)			name
	#define	PROFILE_THIS
	#define PROFILE_NO_HELPER(name)	name
#endif
