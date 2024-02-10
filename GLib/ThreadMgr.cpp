#include "StdAfx.h"
#include "ThreadMgr.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(CThreadMgr);

static const UInt32 MAX_ATTEMPTS = 15;

//////////////////////////////////////////////////////////////////////////

CThreadMgr::CThreadMgr(void) : m_dwTotalThreadCount(0), m_bAutoAdjustSize(true), m_MaxShutdownAttempts(MAX_ATTEMPTS)
{
	m_Threads.Resize(100);
}

CThreadMgr::~CThreadMgr(void)
{
}

//////////////////////////////////////////////////////////////////////////

Bool CThreadMgr::CreateThread(LPTHREAD_START_ROUTINE pfnFunc, 
							  LPVOID vpData /* = NULL */, 
							  Bool bStartNow /* = true */, 
							  CThread** pThread /* = NULL */,
							  UInt32 groupID /* = 0 */,
							  CThread::EThreadPriorityFlags ePriority /* = CThread::eTPF_Default */)
{
	if (!pfnFunc)
	{
		ASSERT(false);
		return false;
	}

	if (!CheckArraySize())
		return false;

	CThread thread(pfnFunc, vpData, bStartNow, ePriority);

	thread.m_ID = m_dwTotalThreadCount;
	thread.SetGroupID(groupID);

	m_Threads[m_dwTotalThreadCount] = thread;

	if (pThread)
		*pThread = &m_Threads[m_dwTotalThreadCount];

	m_dwTotalThreadCount++;
	
	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CThreadMgr::AddThread(CThread& thread)
{
	if (!CheckArraySize())
		return false;

	m_Threads[m_dwTotalThreadCount] = thread;

	m_dwTotalThreadCount++;
	
	return true;
}

//////////////////////////////////////////////////////////////////////////

CThread& CThreadMgr::GetThread(Int32 threadID)
{
	return m_Threads[threadID];
}

//////////////////////////////////////////////////////////////////////////

void CThreadMgr::KillGroup(UInt32 groupID)
{
	for (UInt32 thread = 0; thread < m_dwTotalThreadCount; thread++)
	{
		if (m_Threads[thread].GetGroupID() == groupID)
		{
			m_Threads[thread].Stop();
			m_dwTotalThreadCount--;
		}
	}

	//m_Threads.Resize(100);
}

//////////////////////////////////////////////////////////////////////////

void CThreadMgr::KillAll()
{
	for (UInt32 thread = 0; thread < m_dwTotalThreadCount; thread++)
		m_Threads[thread].Stop();

	m_Threads.Resize(100);
	m_dwTotalThreadCount = 0;
}

//////////////////////////////////////////////////////////////////////////

Bool CThreadMgr::CheckArraySize()
{
	if (m_dwTotalThreadCount >= m_Threads.BufferSize())
	{
		if (m_bAutoAdjustSize)
		{
			m_Threads.Resize(m_dwTotalThreadCount + 1, (m_dwTotalThreadCount > 0 ? true : false));
			return true;
		}

		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CThreadMgr::AllThreadsKilled(UInt32 groupID /* = 0 */, Bool bForceShutdown /* = false */)
{
	for (UInt32 i = 0; i < m_dwTotalThreadCount; i++)
	{
		if (m_Threads[i].IsValid() && m_Threads[i].GetGroupID() == groupID && m_Threads[i].IsRunning())
		{
			if (bForceShutdown && m_Threads[i].GentleShutdown() > m_MaxShutdownAttempts)
			{
				m_Threads[i].Shutdown();
				continue;
			}

			return false;
		}
	}

	return true;
}