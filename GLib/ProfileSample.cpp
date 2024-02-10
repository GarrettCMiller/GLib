///////////////////////////////////////////////////////////////////////////
//		File:				ProfileSample.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A profiler
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\profilesample.h"
#include "Timer.h"
#include <string>

//////////////////////////////////////////////////////////////////////////

IProfilerOutputHandler* CProfileSample::m_OutputHandler = NULL;

CProfileSample::ProfileSample CProfileSample::m_Samples[MAX_PROFILER_SAMPLES];
CProfileSample::ProfileSample CProfileSample::internalSamples[10];

Float	CProfileSample::ms_RootBegin		= 0.0f;
Float	CProfileSample::ms_RootEnd			= 0.0f;
Int32		CProfileSample::ms_OpenSampleCount	= 0;
Int32		CProfileSample::ms_LastOpenedSample	= -1;
Int32		CProfileSample::ms_UniqueID			= 0;
Bool	CProfileSample::ms_bProfilerEnabled	= false;

//////////////////////////////////////////////////////////////////////////

CProfileSample::CProfileSample(const char* name)
{
	m_UniqueID = ms_UniqueID++;
	Create(name);

	if (!internalSamples[0].bIsValid)
	{
		internalSamples[0].Name = "Create()";
		internalSamples[0].bIsValid = true;

		internalSamples[1].Name = "Start()";
		internalSamples[1].bIsValid = true;

		internalSamples[2].Name = "Stop()";
		internalSamples[2].bIsValid = true;
	}
}

//////////////////////////////////////////////////////////////////////////

CProfileSample::~CProfileSample(void)
{
	Stop();
}

//////////////////////////////////////////////////////////////////////////

void CProfileSample::Create(const char* name)
{
	if (!ms_bProfilerEnabled)
		return;

	internalSamples[0].fStartTime = GetTime();

	if (!m_OutputHandler)
		m_OutputHandler = g_ProfileHandler;

	ASSERT(!m_Samples[m_UniqueID].bIsOpen);

	//We did not find the profiler in the list, so make it
	ASSERT(m_UniqueID >= 0);

	m_Samples[m_UniqueID].bIsValid		= true;
	m_Samples[m_UniqueID].Name			= name;
	m_Samples[m_UniqueID].nID			= m_UniqueID;

	m_SampleIndex						= m_UniqueID;
	m_ParentIndex						= ms_LastOpenedSample;

	ms_LastOpenedSample					= m_UniqueID;

	m_Samples[m_UniqueID].ParentCount	= ms_OpenSampleCount;
	//ms_OpenSampleCount++;

	m_Samples[m_UniqueID].bIsOpen		= false;
	m_Samples[m_UniqueID].CallCount		= 0;

	m_Samples[m_UniqueID].fTotalTime	= 0.0f;
	m_Samples[m_UniqueID].fChildTime	= 0.0f;
	m_Samples[m_UniqueID].fStartTime	= 0.0f;//GetTime();

// 	if (m_ParentIndex < 0)
// 		ms_RootBegin = m_Samples[m_UniqueID].fStartTime;

	internalSamples[0].fTotalTime = GetTime() - internalSamples[0].fStartTime;
}

//////////////////////////////////////////////////////////////////////////

void CProfileSample::Start()
{
	if (!ms_bProfilerEnabled)
		return;

	internalSamples[1].fStartTime = GetTime();

	ms_LastOpenedSample					= m_UniqueID;

	m_Samples[m_UniqueID].ParentCount	= ms_OpenSampleCount;
	++ms_OpenSampleCount;

	m_Samples[m_UniqueID].bIsOpen		= true;
	++m_Samples[m_UniqueID].CallCount;

	m_Samples[m_UniqueID].fStartTime	= GetTime();

	if (m_ParentIndex < 0)
		ms_RootBegin = m_Samples[m_UniqueID].fStartTime;

	internalSamples[1].fTotalTime = GetTime() - internalSamples[1].fStartTime;
}

//////////////////////////////////////////////////////////////////////////

void CProfileSample::Stop()
{
	if (!ms_bProfilerEnabled)
		return;

	internalSamples[2].fStartTime = GetTime();

	Float fEndTime = GetTime();

	m_Samples[m_SampleIndex].bIsOpen = false;

	Float fTimeTaken = fEndTime - m_Samples[m_SampleIndex].fStartTime;

	if (m_ParentIndex >= 0)
		m_Samples[m_ParentIndex].fChildTime += fTimeTaken;
	else
		ms_RootEnd = fEndTime;

	m_Samples[m_SampleIndex].fTotalTime += fTimeTaken;
	ms_LastOpenedSample = m_ParentIndex;

	--ms_OpenSampleCount;

	internalSamples[2].fTotalTime = GetTime() - internalSamples[2].fStartTime;
}

//////////////////////////////////////////////////////////////////////////

void CProfileSample::Output(String FileName)
{
	static Timer totalOuputTime(false);

	if (!ms_bProfilerEnabled || !m_Samples[0].bIsValid)
		return;

	//internalSamples

	ASSERT(m_OutputHandler);

	totalOuputTime.Reset();

	m_OutputHandler->BeginOutput(FileName);

	for(UInt32 i = 0; i < MAX_PROFILER_SAMPLES; i++)
	{
		if(m_Samples[i].bIsValid)
		{
			Float sampleTime, percentage;
			//calculate the time spend on the sample itself (excluding children)
			sampleTime = m_Samples[i].fTotalTime - m_Samples[i].fChildTime;
			percentage = ( sampleTime / ( ms_RootEnd - ms_RootBegin ) ) * 100.0f;
			//ASSERT(percentage > 0 && percentage <= 100.0f);

			//add it to the sample's values
			Float totalPc;
			
			totalPc = m_Samples[i].avgPc * m_Samples[i].DataCount;
			totalPc += percentage; 

			m_Samples[i].DataCount++;
			
			m_Samples[i].avgPc = totalPc / m_Samples[i].DataCount;

			if((m_Samples[i].minPc == -1) || (percentage < m_Samples[i].minPc))
				m_Samples[i].minPc = percentage;
			
			if((m_Samples[i].maxPc == -1) || (percentage > m_Samples[i].maxPc))
				m_Samples[i].maxPc = percentage;

			//output these values
			m_OutputHandler->Sample(m_Samples[i].minPc,
									m_Samples[i].avgPc,
									m_Samples[i].maxPc,
									m_Samples[i].CallCount,
									m_Samples[i].Name,
									m_Samples[i].ParentCount);

			//reset the sample for next time
			m_Samples[i].CallCount	= 0;
			m_Samples[i].fTotalTime	= 0;
			m_Samples[i].fChildTime	= 0;
		}
	}

	for (UInt32 i = 0; i < 3; i++)
	{
		m_OutputHandler->Sample(-1,
								internalSamples[i].fTotalTime,
								-1,
								-1,
								internalSamples[i].Name,
								-1);
	}

	m_OutputHandler->Sample(-1,
							totalOuputTime.Get(),
							-1,
							-1,
							"Output",
							0);

	m_OutputHandler->EndOutput();
}

//////////////////////////////////////////////////////////////////////////

void CProfileSample::ResetSample(String name)
{
	for (UInt32 i = 0; i < MAX_PROFILER_SAMPLES; i++)
	{
		if (m_Samples[i].Name == name)
		{
			m_Samples[i].bIsValid = false;
			m_Samples[i].maxPc = m_Samples[i].minPc = -1;
			m_Samples[i].DataCount = 0;
			return;
		}
	}

	ASSERT(false);
}

//////////////////////////////////////////////////////////////////////////

void CProfileSample::ResetAll()
{
	for (UInt32 i = 0; i < MAX_PROFILER_SAMPLES; i++)
	{
		m_Samples[i].bIsValid = false;
		m_Samples[i].maxPc = m_Samples[i].minPc = -1;
		m_Samples[i].DataCount = 0;
	}

	ms_UniqueID = 0;

	ms_LastOpenedSample	= -1;
	ms_OpenSampleCount	= 0;
	ms_RootBegin		= 0.0f;
	ms_RootEnd			= 0.0f;
}

//////////////////////////////////////////////////////////////////////////

Float CProfileSample::GetTime()
{
	return Timer::ms_GlobalTimer.Get();
}

//////////////////////////////////////////////////////////////////////////