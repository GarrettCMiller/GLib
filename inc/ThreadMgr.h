#pragma once

#include "singleton.h"
#include "DynamicArray.h"

#include "Thread.h"

//////////////////////////////////////////////////////////////////////////

typedef CDynamicArray<CThread> TThreadArray;

class CThreadMgr : public Singleton<CThreadMgr>
{
public:
	CThreadMgr(void);
	~CThreadMgr(void);

	Bool CreateThread(LPTHREAD_START_ROUTINE pfnFunc, LPVOID vpData = NULL, 
						Bool bStartNow = true, CThread** pThread = NULL,
						UInt32 groupID = 0,
						CThread::EThreadPriorityFlags ePriority = CThread::eTPF_Default);

	Bool AddThread(CThread& thread);

	CThread& GetThread(Int32 threadID);

	UInt32 GetThreadCount() const
	{
		return m_dwTotalThreadCount;
	}

	void KillAll();

	void KillGroup(UInt32 groupID);

	void KillThread(CThread& thread)
	{
		m_Threads[thread.m_ID].Invalidate();
	}

	bool AllThreadsKilled(UInt32 groupID = 0, Bool bForceShutdown = false);

	void SetMaxShutdownAttempts(UInt32 maxAttempts)
	{
		m_MaxShutdownAttempts = maxAttempts;
	}

protected:

	Bool			CheckArraySize();

	/*CDynamicArray<*/TThreadArray/*>*/	m_Threads;
	UInt32						m_dwTotalThreadCount;
	UInt32						m_MaxShutdownAttempts;

	Bool						m_bAutoAdjustSize;

};

#define g_ThreadMgr	CThreadMgr::GetInstance()