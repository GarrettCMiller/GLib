#include "StdAfx.h"
#include "Thread.h"

//////////////////////////////////////////////////////////////////////////

UInt32 CThread::mk_NextID = 0;

//////////////////////////////////////////////////////////////////////////

CThread::CThread(LPTHREAD_START_ROUTINE pfnFunc /* = NULL */, 
				 LPVOID vpData /* = NULL */, 
				 Bool bStartNow /* = true */, 
				 EThreadPriorityFlags ePriority /* = eTPF_Default */) 
				 : m_bIsValid(false), m_bIsRunning(false), m_hHandle(NULL), 
				 m_ShutdownAttempts(0), m_bShutdownFlag(false), m_ID(static_cast<UInt32>(-1)), m_GroupID(0)
{
	if (pfnFunc)//m_ID			= mk_NextID++;
		Init(pfnFunc, vpData, bStartNow, ePriority);
}

CThread::~CThread(void)
{
	//Shutdown();
}

//////////////////////////////////////////////////////////////////////////

Bool CThread::Init(LPTHREAD_START_ROUTINE pfnFunc /* = NULL */, 
				   LPVOID vpData /* = NULL */, 
				   Bool bStartNow /* = true */, 
				   EThreadPriorityFlags ePriority /* = eTPF_Default */)
{
	ASSERT(pfnFunc);
	//if (!pfnFunc)
	//	return false;

	m_hHandle = ::CreateThread(NULL, 0,
								pfnFunc,
								vpData,
								(bStartNow ? 0 : CREATE_SUSPENDED),
								0);

	if (m_hHandle)
	{
		m_bIsValid		= true;
		m_bIsRunning	= bStartNow;
		m_pfnFunction	= pfnFunc;

		if (ePriority != eTPF_Default)
			SetPriority(ePriority);
		
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

Int32 CThread::Start()
{
	Int32 ret = static_cast<Int32>(-1);

	if (m_bIsValid && !m_bIsRunning)
		ret = ResumeThread(m_hHandle);

	if (ret > 0)
		m_bIsRunning = true;

	return ret;
}

//////////////////////////////////////////////////////////////////////////

Int32 CThread::Stop()
{
	Int32 ret = static_cast<Int32>(-1);

	if (m_bIsValid && m_bIsRunning)
		ret = SuspendThread(m_hHandle);

	if (ret > 0)
		m_bIsRunning = false;

	return ret;
}

//////////////////////////////////////////////////////////////////////////

void CThread::Shutdown()
{
	if (m_bIsValid && m_bIsRunning)
	{
		if (reinterpret_cast<UInt32>(m_hHandle) != 0xFeeeFeee)
			CloseHandle(m_hHandle);
	}

	m_bIsRunning = false;
	m_bIsValid = false;

	m_hHandle = NULL;
}

//////////////////////////////////////////////////////////////////////////

UInt32 CThread::GentleShutdown()
{
	m_bShutdownFlag = true;
	return ++m_ShutdownAttempts;
}

//////////////////////////////////////////////////////////////////////////